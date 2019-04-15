// PWMtest.c
// Runs on TM4C123
// Test project for motor board
// ***************************************************
// ************remove R9 and R10**********************
// ***************************************************

// Version 6 hardware (use program main)
// to go forward on right motor
// PB7 A+  regular GPIO level high (1)
// PB6 A-  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// to go backward on right motor
// PB7 A+  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// PB6 A-  regular GPIO level high (1)
// coast on right motor (fast decay)
// PB7 A+  regular GPIO level low (0)
// PB6 A-  regular GPIO level low (0)
// to go forward on left motor
// PB5 B+  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// PB4 B-  regular GPIO level high (1) 
// to go backward on left motor
// PB5 B+  regular GPIO level high (1)
// PB4 B-  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// coast on left motor (fast decay)
// PB5 B+  regular GPIO level low (0)
// PB4 B-  regular GPIO level low (0)

// Version 5 hardware (use program main5)
// PB7 A+  PWM 100 Hz, right motor, PWM positive logic
// PB6 A-  regular GPIO, right motor, 0 means forward
// PB5 B+  PWM 100 Hz, left motor, PWM negative logic
// PB4 B-  regular GPIO, left motor, 1 means forward
// PD0 is servo A, 20ms period, pulse time 0.5 to 2.5ms
// PD1 is servo B (not used), 20ms period, pulse time 0.5 to 2.5ms
// Servo period = 20ms = 25000 PWM clocks (1.25MHz PWM)
//   min pulse  = 0.5ms = 625 PWM clocks
//   mid pulse  = 1.5ms = 1875 PWM clocks
//   max pulse  = 2.5ms = 3125 PWM clocks
//   because of rack-pinion restrict to 1250 to 2375
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// February 15, 2017

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2017
   Program 8.4, Section 8.3

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "PLL.h"
#include "MotorControl.h"
#include "tm4c123gh6pm.h"

void WaitForInterrupt(void);  // low power mode
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
// connect PF1 to PB7
void PortF_Init(void);

void DelayWait10ms(uint32_t n);
void WaitForRelease(void);
void WaitForTouch(void);






// SW1 cycles through seven speeds 400,2400,4400,6400,8400,10400,12400
int main(void){ // version 6 with DRV8848
	int power = 0, servoangle = 5;
	int8_t Steering = -45;
	PLL_Init(Bus80MHz);  // bus clock at 80 MHz
	PortF_Init();
	Motor_Initialization();
	Motor_Direction(BACKWARD);
	//Motor_Speed(99);
	//Motor_Steer(-45);
	while(1){
		WaitForTouch();
		if(PF0==0){
			if (Steering == 45){ servoangle = -5;}
			if (Steering == -45){ servoangle = 5;}
			Steering += servoangle;
			Motor_Steer((int8_t)Steering);    // SERVOMIN to SERVOMAX
			GPIO_PORTF_DATA_R ^= GREEN;
		}
		if(PF4==0){
			Motor_Speed(power);
			power++;
			GPIO_PORTF_DATA_R ^= RED;
		  }
		WaitForRelease();
    }
    
}

void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  while((SYSCTL_PRGPIO_R&0x0020) == 0){};// ready?
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R = 0x01;          // enable commit for PF0

  GPIO_PORTF_DIR_R &= ~0x11;       // make PF4 input
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x1F;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x1F;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PUR_R |=0x11;         // pullup for two switches
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF00000)+0x00000000;
  GPIO_PORTF_AMSEL_R &= ~0x1F;     // disable analog functionality on PF
  LEDS = 0;                        // turn all LEDs off
}
void WaitForTouch(void){
  while((PF0==0x01)&&(PF4==0x10)){};
}

void WaitForRelease(void){
  while((PF0==0x01)&&(PF4==0x10)){};  // wait for switch touch
  DelayWait10ms(2); // debounce
  while((PF0!=0x01)||(PF4!=0x10)){};  // wait for both release
  DelayWait10ms(2); // debounce
}
// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
      time--;
    }
    n--;
  }
}
