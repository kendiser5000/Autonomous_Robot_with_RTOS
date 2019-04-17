/**
* @file main.c
* @brief Main file for project, contains autonomous algorithm, also gets sensor data
* @author Sikender
* @date 4/14/2019
**/

#include "OS.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"
#include "Interpreter.h"
#include <stdint.h>
#include "can0.h"
#include "ToF.h"


/* Port E GPIO */
#define PE0  (*((volatile unsigned long *)0x40024004))
#define PE1  (*((volatile unsigned long *)0x40024008))
#define PE2  (*((volatile unsigned long *)0x40024010))
#define PE3  (*((volatile unsigned long *)0x40024020))
	
/* Port F GPIO: LEDS */
#define PF0  (*((volatile uint32_t *)0x40025004))
#define PF1  (*((volatile uint32_t *)0x40025008))
#define PF2  (*((volatile uint32_t *)0x40025010))
#define PF3  (*((volatile uint32_t *)0x40025020))

#define SENSOR1 0
#define SENSOR2 3
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode


unsigned char LaserData1[4], LaserData2[4];

Sema4Type LaserData1Rdy, LaserData2Rdy;



/** PortE_Init
 * @brief Port E init for GPIO debugging
*/
void GPIO_PortE_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x10;       // activate port E
  while((SYSCTL_RCGCGPIO_R&0x10)==0){};      
  GPIO_PORTE_DIR_R |= 0x0F;    // make PE3-0 output heartbeats
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;     // enable digital I/O on PE3-0
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;;      // disable analog functionality on PF
}

/** PortF_Init
 * @brief Port F init for GPIO debugging
*/
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



/** laserToArray
 * @brief Helper to put laser data into array
 * @parameter value number to convert
 * @parameter array array to put data in
*/
void laserToArray(int value, unsigned char* array){
	array[0] = (value & 0xFF)		>> 0;
	array[1] = (value & 0xFF00) 	>> 8;
	array[2] = (value & 0xFF0000) 	>> 16;
	array[3] = (value & 0xFF000000) >> 24;
}


/** GetLaser1
 * @brief Port F init for GPIO debugging
*/
void GetLaser1(void){
	int distance;
	while(1){
		distance = ToF_Measure(SENSOR1);
		laserToArray(distance, LaserData1);
		OS_Signal(&LaserData1Rdy);
		UART_OutString("L1: ");
		UART_OutUDec(distance);
		UART_OutString("mm\r\n");
		OS_Signal(&LaserData1Rdy);
	}
}


/** GetLaser2
 * @brief Port F init for GPIO debugging
*/
void GetLaser2(void){
	int distance;
	while(1){
		distance = ToF_Measure(SENSOR2);
		laserToArray(distance, LaserData1);
		UART_OutString("L2: ");
		UART_OutUDec(distance);
		UART_OutString("mm\r\n");
		OS_Signal(&LaserData2Rdy);
	}
}


/** GetLaser1
 * @brief Get Laser Sensor 1 Data
*/
void SendLaser1(void){
	while(1){
		OS_Wait(&LaserData1Rdy);
		CAN0_SendData(LaserSensor1, LaserData1);		
	}
}


/** GetLaser2
 * @brief Get Laser Sensor2 Data
*/
void SendLaser2(void){
	while(1){
		OS_Wait(&LaserData2Rdy);
		CAN0_SendData(LaserSensor2, LaserData2);	
	}
}


/** Init IO
 * @brief Sensors, Debug Ports, CAN
*/
void IOInit(void){
	GPIO_PortF_Init();
	GPIO_PortE_Init();
	CAN0_Open();
	ToF_Init(SENSOR1);
	ToF_Init(SENSOR2);
}


/** WrapInterpreter
 * @brief Wrapper thread to call interpreter
*/
void WrapInterpreter(void){
	while(1){
		Interpreter();
	}
	
}


unsigned long NumCreated;   // number of foreground threads created
int main(void){
	/* OS Initializations */
	OS_Init();           
	//********initialize communication channels
	OS_MailBox_Init();
	OS_Fifo_Init(128);    // ***note*** 4 is not big enough*****//128
	IOInit();
	NumCreated = 0;
	// create initial foreground threads
	//NumCreated += OS_AddThread(&WrapInterpreter,128,2); 
	NumCreated += OS_AddThread(&GetLaser1,128,2); 
	NumCreated += OS_AddThread(&GetLaser2,128,2); 
	NumCreated += OS_AddThread(&SendLaser1,128,2); 
	NumCreated += OS_AddThread(&SendLaser2,128,2); 

	OS_InitSemaphore(&LaserData1Rdy, 0);
	OS_InitSemaphore(&LaserData2Rdy, 0);
	OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
	return 0;            // this never executes
}

/** HardFault_Handler
 * @brief Make LED White in case of hard fault
*/
void HardFault_Handler(void){
	PF1 = 0x02;
	PF2 = 0x04;
	PF3 = 0x08;
	while(1);
}
