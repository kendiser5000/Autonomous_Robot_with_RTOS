/** @file Interpreter.c
 * @brief Runs on TM4C123
 * UART Command Line Interface
 * @date 2/9/2019
 * @author Sikender Ashraf and Sijin Woo
 */

#include "MotorControl.h"
#include "OS.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "Interpreter.h"
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


int LaserDistance1, LaserDistance2, LaserDistance3, LaserDistance4;

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



/** SendLaser1
 * @brief Helper to put laser data into array
 * @parameter value number to convert
 * @parameter array array to put data in
*/
int arrayToDistance(unsigned char* array){
	int data = 0;
	data |= array[0]	<< 0;
	data |= array[1]	<< 8;
	data |= array[2]	<< 16;
	data |= array[3]	<< 24;
	return data;
}


/** GetLaser1
 * @brief Port F init for GPIO debugging
*/
void GetLaser1(void){
	while(1){
		LaserDistance1 = ToF_Measure(SENSOR1);
		OS_Signal(&LaserData1Rdy);
	}
}


/** GetLaser2
 * @brief Port F init for GPIO debugging
*/
void GetLaser2(void){
	while(1){
		LaserDistance2 = ToF_Measure(SENSOR2);
		OS_Signal(&LaserData2Rdy);
	}
}


/** ProcessCAN
 * @brief process CAN data and set proper values
*/
void ProcessCAN(void){
	CANID id;
	unsigned char data[4];
	while(1){
		CAN0_GetMail(&id, data);
		// handle data depending on ID type
		switch(id){
			case (LaserSensor1):
				LaserDistance1 = arrayToDistance(data);
				break;
			case (LaserSensor2):
				LaserDistance1 = arrayToDistance(data);
				break;
			case (LaserSensor3):
				LaserDistance1 = arrayToDistance(data);
				break;
			case(LaserSensor4):
				LaserDistance1 = arrayToDistance(data);
				break;
			case(Motors):
				break;
			case(TEST):
				UART_OutUHex(arrayToDistance(data));
				break;
		}
	
	}

}



/** Algorithm
 * @brief Algorithm for autonomous car
*/
void Algorithm(void){
	int motorLeftSpeed = 0, motorRightSpeed = 0, steeringAngle = 0;
	enum Direction motorLeftDir = COAST, motorRightDir = COAST;
	Motor_Speed(0);
	Motor_DirectionLeft(motorLeftDir);
	Motor_DirectionRight(motorRightDir);
	Motor_SpeedLeft(motorLeftSpeed);
	Motor_SpeedRight(motorRightSpeed);
	Motor_Steer(steeringAngle);
	
	while(1){
		
	
	}

}


/** Init IO
 * @brief Sensors, Debug Ports, CAN
*/
void IOInit(void){
	GPIO_PortF_Init();
	GPIO_PortE_Init();
	Motor_Initialization();
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
	NumCreated += OS_AddThread(&Algorithm,128,2); 
	NumCreated += OS_AddThread(&ProcessCAN,128,1); 


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
