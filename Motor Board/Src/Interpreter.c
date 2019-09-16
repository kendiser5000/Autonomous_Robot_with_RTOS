/** @file Interpreter.c
 * @brief Runs on TM4C123
 * UART Command Line Interface
 * @date 2/9/2019
 * @author Sikender Ashraf and Sijin Woo
 */
 
#include <stdint.h>
#include "UART.h"
#include "tm4c123gh6pm.h"
#include "OS.h"
#include "ToF.h"
#include "CAN0.h"


unsigned char UserInput[4];

/** newLine
 *	Create new line on UART interface
 * @author Sikender & Sijin
 * @date 2/04/2019
*/
void newLine(void){
	OutCRLF();
}


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
	UART_OutString ("1: Measure ToF Sensor 1"); 
	newLine();
	UART_OutString ("2: Measure ToF Sensor 2");
	newLine();
	UART_OutString ("3: Send TEST CAN data"); 
	newLine();
	UART_OutString ("Selection: ");
	char selection = UART_InChar();
	// Error checking
	while ((selection != '1') && (selection != '2') && (selection != '3')){
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
			UART_OutString("ToF Sensor 1 Measurement: ");
			UART_OutUDec(ToF_Measure(0));
			UART_OutString("mm\r\n");
			break;
		case '2': 
			UART_OutString("ToF Sensor 2 Measurement: ");
			UART_OutUDec(ToF_Measure(3));
			UART_OutString("mm\r\n");
			break;
		case '3': 
			UART_OutString("Enter Data to Send: ");
			UART_InString((char*)UserInput, 4);
			UART_OutString("Data Sent.");
			break;
		
	}
	newLine();
}



