// Lab2.c
// Runs on LM4F120/TM4C123
// Real Time Operating System for Labs 2 and 3
// Lab2 Part 1: Testmain1 and Testmain2
// Lab2 Part 2: Testmain3 Testmain4  and main
// Lab3: Testmain5 Testmain6, Testmain7, and main (with SW2)

// Jonathan W. Valvano 2/20/17, valvano@mail.utexas.edu
// EE445M/EE380L.12
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file

// LED outputs to logic analyzer for OS profile 
// PF1 is preemptive thread switch
// PF2 is periodic task, samples PD3
// PF3 is SW1 task (touch PF4 button)

// Button inputs
// PF0 is SW2 task (Lab3)
// PF4 is SW1 button input

// Analog inputs
// PD3 Ain3 sampled at 2k, sequencer 3, by DAS software start in ISR
// PD2 Ain5 sampled at 250Hz, sequencer 0, by Producer, timer tigger

#include "OS.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"
#include "Interpreter.h"
#include <stdint.h>
#include <stdbool.h>
#include "Timer3.h"
#include "can0.h"

#include "Ping.h"
#include "Servo.h"
#include "MotorPWM.h"

#include "IR.h"
#include "ToF.h"




#define PE0  (*((volatile unsigned long *)0x40024004))
#define PE1  (*((volatile unsigned long *)0x40024008))
#define PE2  (*((volatile unsigned long *)0x40024010))
#define PE3  (*((volatile unsigned long *)0x40024020))
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF3                     (*((volatile uint32_t *)0x40025020))

#define SERVO_PERIOD 	1
#define SERVO_DUTY		1

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
uint8_t XmtData[4];
uint8_t RcvData[4];
uint32_t RcvCount=0;
uint8_t sequenceNum=0; 
unsigned long NumCreated;   // number of foreground threads created



void PortE_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x10;       // activate port E
  while((SYSCTL_RCGCGPIO_R&0x10)==0){};      
  GPIO_PORTE_DIR_R |= 0x0F;    // make PE3-0 output heartbeats
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;     // enable digital I/O on PE3-0
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;;      // disable analog functionality on PF
}

void GPIO_PortF_Init(void){   
	
  SYSCTL_RCGCGPIO_R |= 0x20;     // 1) activate Port F
  while((SYSCTL_PRGPIO_R & 0x20)!=0x20){}; // wait to finish activating     
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;// 2a) unlock GPIO Port F Commit Register
  GPIO_PORTF_CR_R = 0x0F;        // 2b) enable commit for PF0-PF3    
  GPIO_PORTF_AMSEL_R &= ~0x0F;   // 3) disable analog functionality on PF0-PF3     
  GPIO_PORTF_PCTL_R &= ~0x000FFFF;// 4) configure PF0-PF3 as GPIO
  GPIO_PORTF_DIR_R = 0x0F;       // 5) make PF0-3 output                       
  GPIO_PORTF_AFSEL_R &= ~0x0F;        // 6) disable alt funct on PF0-PF3
  GPIO_PORTF_DEN_R = 0x0F;       // 7) enable digital I/O on PF0-PF3
}


void WrapInterpreter(void){
	while(1){
		Interpreter();
	}
	
}




void UserTask(void){
  XmtData[0] = PF0<<1;  // 0 or 2
  XmtData[1] = PF4>>2;  // 0 or 4
  XmtData[2] = 0;       // unassigned field
  XmtData[3] = sequenceNum;  // sequence count
  //CAN_SendData(XmtData);
  sequenceNum++;
}


int main(void){    // realmain
	OS_Init();           // initialize, disable interrupts
	GPIO_PortF_Init();
	//********initialize communication channels
	OS_MailBox_Init();
	OS_Fifo_Init(128);    // ***note*** 4 is not big enough*****//128
	
	Servo_Initilization(SERVO_PERIOD, SERVO_DUTY);
	ToF_Init(1);
	IR_Init(3);
	Ping_Init();
	OS_AddPeriodicThread(&Ping_StartHCSR04, 80000000/20, 0); //20 Hz data Collection
	
	CAN0_Open();
	NumCreated = 0 ;
	// create initial foreground threads
	NumCreated += OS_AddThread(&WrapInterpreter,128,2); //2


	OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
	return 0;            // this never executes
}


void HardFault_Handler(void){
	PF1 = 0x02;
	PF2 = 0x04;
	PF3 = 0x08;
	while(1);
}
