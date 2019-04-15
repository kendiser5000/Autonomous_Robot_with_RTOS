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
	UART_OutString ("2: ");
	newLine();
	UART_OutString ("3: "); 
	newLine(); 
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
			
			break;
		case '2': 
			 
			break;
		case '3': 
			
			break;
	}
	newLine();
}



