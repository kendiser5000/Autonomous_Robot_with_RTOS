/** @file Interpreter.c
 * @brief Runs on TM4C123
 * UART Command Line Interface
 * @date 2/9/2019
 * @author Sikender Ashraf and Sijin Woo
 */
 
#include <stdint.h>
#include "UART.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "OS.h"
#include "ToF.h"
#include "IR.h"
#include "Ping.h"
#include "CAN0.h"
#include "ADC.h"


/** MAXSAMPLES
 * Max number of Samples for ADC Collect
 */
#define MAXSAMPLES 1000
//static uint16_t Buffer[MAXSAMPLES];

/** newLine
 *	Create new line on UART interface
 * @author Sikender & Sijin
 * @date 2/04/2019
*/
void newLine(void){
	OutCRLF();
}

char* USONICStrings[3] = {"US0: ", "US1: ", "US2: "};
uint32_t USONICValues[NUM_USONIC];

/** Interpreter_RealMain
 * Interpreter module for lab 2 realmain
 * Shows datalost, PID calculations finished, digital filter calculators finished
 * @parameter DataLost
 * @parameter PIDWork
 * @parameter FilterWork
 * @author Sikender & Sijin
 * @date 2/04/2019
*/
void Interpreter(){
	
	UART_OutString ("Choose measurement:"); 
	newLine();
	UART_OutString ("1: SendCan"); 
	newLine();
	UART_OutString ("2: Measure ToF");
	newLine();
	UART_OutString ("3: Measure IR"); 
	newLine(); 
	UART_OutString ("4: Measure Ultrasonic"); 
	newLine(); 
	//UART_OutString ("5: PWM"); 
	//newLine(); 
	//UART_OutString ("4: Measure Ultrasonic"); 
	//newLine(); 
	newLine();
	UART_OutString ("Selection: ");
	static short i = 0;
	char selection = UART_InChar();
	// Error checking
	while ((selection != '1') && (selection != '2') && (selection != '3') && (selection != '4')){
			newLine(); newLine();
			UART_OutString ("Invalid Choice, try again"); 
			newLine(); 
			UART_OutString ("Selection: ");
			selection = UART_InChar();
		}
	newLine(); 
	newLine();
	// Output text for selection
	switch (selection){
		case '1': 
			UART_OutString ("Sending data onto CAN"); 
			uint8_t data[4] = {0x04, 0x03, 0x02, 0x01};
			unsigned char CanData[4];
			((unsigned short*)CanData)[0] = i++;
			i %= 4;
			CAN0_SendData(IRSensor, CanData);
			newLine();
			break;
		case '2': 
			UART_OutString("ToF Sensor 1 Measurement: ");
			UART_OutUDec(ToF_Measure(1));
			UART_OutString("mm\r\n");
			break;
		case '3': 
			//IR_Init(3);
			UART_OutString("IR Sensor Measurement: ");
			UART_OutUDec(IR_getValue());
			UART_OutString("mm\r\n");
			break;
		case '4':
			UART_OutString("Ultrasonic Sensor Measurement: ");
			uint8_t message[8];
			Ping_GetData(USONICValues);
			for(int i=0; i<NUM_USONIC; i++){
				message[2*i] = (USONICValues[i]&0x00FF);
				message[2*i+1] = ((USONICValues[i]&0xFF00)>>8);
				UART_OutUDec(USONICValues[i]);
			}
			UART_OutString("mm\r\n");
			break;
	}
	newLine();
}



