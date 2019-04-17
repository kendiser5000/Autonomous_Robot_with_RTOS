// PWM.c
// Runs on TM4C123
// Use PWM0A/PB6 and PWM0B/PB7 to generate pulse-width modulated outputs
// Use PWM1A/PB4 and PWM1B/PB5 to generate pulse-width modulated outputs
// Use PWM3A/PD3 to generate pulse-width modulated output

// Daniel Valvano
// February 15, 2018
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
// PD1 was servo B, but no software written for PD1

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
#include "pwm.h"
#include "tm4c123gh6pm.h"
#define PWM_0_GENA_ACTCMPAD_ONE 0x000000C0  // Set the output signal to 1
#define PWM_0_GENA_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0
#define PWM_0_GENB_ACTCMPBD_ONE 0x00000C00  // Set the output signal to 1
#define PWM_0_GENB_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0

#define SYSCTL_RCC_USEPWMDIV    0x00100000  // Enable PWM Clock Divisor
#define SYSCTL_RCC_PWMDIV_M     0x000E0000  // PWM Unit Clock Divisor
#define SYSCTL_RCC_PWMDIV_2     0x00000000  // /2

#define PB7       (*((volatile uint32_t *)0x40005200))
#define PB6       (*((volatile uint32_t *)0x40005100))
#define PB5       (*((volatile uint32_t *)0x40005080))
#define PB4       (*((volatile uint32_t *)0x40005040))

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB7 (0 or 1)
// Output on PB6/M0PWM0
// GPIO on   PB7 controls direction
void Right_Init(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on PB6
  GPIO_PORTB_AFSEL_R &= ~0x80;          // disable alt funct on PB7
  GPIO_PORTB_PCTL_R &= ~0xFF000000;     // configure PB6 as PWM0
  GPIO_PORTB_PCTL_R |= 0x04000000;      // PB7 regular
  GPIO_PORTB_AMSEL_R &= ~0xC0;          // disable analog functionality on PB6
  GPIO_PORTB_DIR_R |= 0xC0;             // PB7, PB6 output
  GPIO_PORTB_DEN_R |= 0xC0;             // enable digital I/O on PB6
  SYSCTL_RCC_R = 0x001A0000 |           // 3) use PWM divider 
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PB6 goes low on LOAD
  // PB6 goes high on CMPA down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R &= ~0x00000002;         // disable PB7/M0PWM1
  PWM0_ENABLE_R |= 0x00000001;          // enable PB6/M0PWM0
  if(direction){
    PB7 = 0x80;
  }else{
    PB7 = 0;
  }
}
// change duty cycle of PB6
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB7(0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Right_Duty(uint16_t duty, int direction){
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
  if(direction){
    PB7 = 0x80;
  }else{
    PB7 = 0;
  }
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB6 (0 or 1)
// Output on PB7/M0PWM0B
// GPIO on   PB6 controls direction
void Right_InitB(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x80;           // enable alt funct on PB7
  GPIO_PORTB_AFSEL_R &= ~0x40;          // disable alt funct on PB6
  GPIO_PORTB_PCTL_R &= ~0xFF000000;     // configure PB7 as PWM0B = M0PWM1
  GPIO_PORTB_PCTL_R |= 0x40000000;      // PB6 regular
  GPIO_PORTB_AMSEL_R &= ~0xC0;          // disable analog functionality on PB7,PB6
  GPIO_PORTB_DIR_R |= 0xC0;             // PB6 PB7 output
  GPIO_PORTB_DEN_R |= 0xC0;             // enable digital I/O on PB7,PB6
  SYSCTL_RCC_R = 0x001A0000 |           // 3) use PWM divider 
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENB_R = 0xC08;                // low on LOAD, high on CMPB down
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R &= ~0x00000001;         // disable PB6/M0PWM0
  PWM0_ENABLE_R |= 0x00000002;          // enable PB7/M0PWM1
  if(direction){
    PB6 = 0x40;
  }else{
    PB6 = 0;
  }
}
// change duty cycle of PB7
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB6(0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Right_DutyB(uint16_t duty, int direction){
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
  if(direction){
    PB6 = 0x40;
  }else{
    PB6 = 0;
  }
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB5 (0 or 1)
// Output on PB4/M0PWM1A
// GPIO on   PB5 controls direction
void Left_Init(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x10;           // enable alt funct on PB4
  GPIO_PORTB_AFSEL_R &= ~0x20;          // disable alt funct on PB5
  GPIO_PORTB_PCTL_R &= ~0x00FF0000;     // configure PB4 as PWM0
  GPIO_PORTB_PCTL_R |= 0x00040000;      // PB5 regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x30;          // disable analog functionality on PB4,5
  GPIO_PORTB_DIR_R |= 0x30;             // PB5 PB4 outputs
  GPIO_PORTB_DEN_R |= 0x30;             // enable digital I/O on PB4, PB5
  SYSCTL_RCC_R = 0x001A0000 |           // 3) use PWM divider 
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_1_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_1_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PB4 goes low on LOAD
  // PB4 goes high on CMPA down
  PWM0_1_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_1_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R &= ~0x00000008;         // disable PB5/M0PWM1B
  PWM0_ENABLE_R |= 0x00000004;          // enable PB4/M0PWM1A is PWM2
  if(direction){
    PB5 = 0x20;
  }else{
    PB5 = 0;
  }
}
// change duty cycle of PB4
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB5 (0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Left_Duty(uint16_t duty, int direction){
  PWM0_1_CMPA_R = duty - 1;    // 6) count value when output rises
  if(direction){
    PB5 = 0x20;
  }else{
    PB5 = 0;
  }
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB4 (0 or 1)
// Output on PB5/M0PWM1B
// GPIO on   PB4 controls direction
void Left_InitB(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x20;           // enable alt funct on PB5
  GPIO_PORTB_AFSEL_R &= ~0x10;          // disable alt funct on PB4
  GPIO_PORTB_PCTL_R &= ~0x00FF0000;     // configure PB5 as PWM3
  GPIO_PORTB_PCTL_R |= 0x00400000;      // PB4 regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x30;          // disable analog functionality on PB4,5
  GPIO_PORTB_DIR_R |= 0x30;             // PB5 PB4 outputs
  GPIO_PORTB_DEN_R |= 0x30;             // enable digital I/O on PB4, PB5
  SYSCTL_RCC_R = 0x001A0000 |           // 3) use PWM divider 
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_1_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_1_GENB_R = 0xC08;                // low on LOAD, high on CMPB down
  // PB5 goes low on LOAD
  // PB45 goes high on CMPB down
  PWM0_1_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_1_CMPB_R = duty - 1;             // 6) count value when output rises
  PWM0_1_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R &= ~0x00000004;         // disable PB4/M0PWM1A
  PWM0_ENABLE_R |= 0x00000008;          // enable PB5/M0PWM1B is PWM3
  if(direction){
    PB4 = 0x10;
  }else{
    PB4 = 0;
  }
}
// change duty cycle of PB5
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB4 (0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Left_DutyB(uint16_t duty, int direction){
  PWM0_1_CMPB_R = duty - 1;    // 6) count value when output rises
    if(direction){
    PB4 = 0x10;
  }else{
    PB4 = 0;
  }
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
// Output on PD0/M0PWM3A is PWM6
void Servo_Init(uint16_t period, uint16_t duty){
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x08;            // 2) activate port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};
  GPIO_PORTD_AFSEL_R |= 0x01;           // enable alt funct on PD0
  GPIO_PORTD_PCTL_R &= ~0x0000000F;     // configure PD0 as PWM0
  GPIO_PORTD_PCTL_R |= 0x00000004;
  GPIO_PORTD_AMSEL_R &= ~0x01;          // disable analog functionality on PD0
  GPIO_PORTD_DEN_R |= 0x01;             // enable digital I/O on PD0
  SYSCTL_RCC_R = 0x001A0000 |           // 3) use PWM divider 
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_3_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_3_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PD0 goes low on LOAD
  // PD0 goes high on CMPA down
  PWM0_3_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_3_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R |= 0x00000040;          // enable PD0/M0PWM3A is PWM6
}
// change duty cycle of PD0
// Inputs: period was set in call to Init
//         duty is in 800 ns units
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Servo_Duty(uint16_t duty){
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
}

uint16_t RightPeriod; // Right motor period in 800ns units
uint16_t LeftPeriod;  // Left motor period in 800ns units
enum Direction RightDirection=COAST;
enum Direction LeftDirection=COAST;
// Motor board version 6 with DRV8848 dual H-bridge
void DRV8848_RightStop(void){
  RightDirection = COAST;
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R &= ~0xC0;          // disable alt funct on PB6,PB7
  GPIO_PORTB_PCTL_R &= ~0xFF000000;     // configure PB6,PB7 as GPIO
  GPIO_PORTB_AMSEL_R &= ~0xC0;          // disable analog functionality on PB7,PB6
  GPIO_PORTB_DIR_R |= 0xC0;             // PB6 PB7 output
  GPIO_PORTB_DEN_R |= 0xC0;             // enable digital I/O on PB7,PB6
  PB6 = 0x00;
  PB7 = 0x00;
}

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is BACKWARD, FORWARD, COAST
// Configures output on PB6, PB7 (one is GPIO other is PWM)
// Motor board version 6 with DRV8848 dual H-bridge
void DRV8848_RightInit(uint16_t period, uint16_t duty, enum Direction direction){
  RightDirection = direction;
  RightPeriod = period;
  if(direction == COAST){
    DRV8848_RightStop();
  }else if(direction == FORWARD){
    Right_Init(period, period-duty, 1);  // PB6 is negative logic duty, PB7 is 1
  }else{
    Right_InitB(period, period-duty, 1); // PB7 is negative logic duty, PB6 is 1
  }
}

// change duty cycle of right motor (PB7,PB6)
// Inputs: period was set in call to Right_InitDRV8848
//         duty is in 800 ns units
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// Must call Right_InitDRV8848  
// - once initially before calling Right_DutyDRV8848 
// - to change direction
// Motor board version 6 with DRV8848 dual H-bridge
void DRV8848_RightDuty(uint16_t duty){
  if((RightDirection==COAST)||(duty<2)||(duty>(RightPeriod-1))){
    return; // illegal
  }
  if(RightDirection == FORWARD){
    Right_Duty(RightPeriod-duty, 1);  // PB6 is negative logic duty, PB7 is 1
  }else{
    Right_DutyB(RightPeriod-duty, 1); // PB7 is negative logic duty, PB6 is 1
  }
  
}
 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is BACKWARD, FORWARD, COAST
// Configures output on PB4, PB5 (one is GPIO other is PWM)
// Motor board version 6 with DRV8848 dual H-bridge
void DRV8848_LeftInit(uint16_t period, uint16_t duty, enum Direction direction){
  LeftDirection = direction;
  LeftPeriod = period;
  if(direction == COAST){
    DRV8848_LeftStop();
  }else if(direction == FORWARD){
    Left_InitB(period, period-duty, 1); // PB5 is negative logic duty, PB4 is 1
  }else{
    Left_Init(period, period-duty, 1);  // PB4 is negative logic duty, PB5 is 1
  }
}

// Motor board version 6 with DRV8848 dual H-bridge
void DRV8848_LeftStop(void){
  LeftDirection = COAST;
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R &= ~0x30;          // disable alt funct on PB4,PB5
  GPIO_PORTB_PCTL_R &= ~0x00FF0000;     // configure PB4,PB5 as GPIO
  GPIO_PORTB_AMSEL_R &= ~0x30;          // disable analog functionality on PB5,PB4
  GPIO_PORTB_DIR_R |= 0x30;             // PB5 PB4 output
  GPIO_PORTB_DEN_R |= 0x30;             // enable digital I/O on PB5,PB4
  PB4 = 0x00;
  PB5 = 0x00;
}

// change duty cycle of right motor (PB5,PB4)
// Inputs: period was set in call to Left_InitDRV8848
//         duty is in 800 ns units
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// Must call Left_InitDRV8848  
// - once initially before calling Left_DutyDRV8848 
// - to change direction
 // Motor board version 6 with DRV8848 dual H-bridge
void DRV8848_LeftDuty(uint16_t duty){
   if((LeftDirection==COAST)||(duty<2)||(duty>(LeftPeriod-1))){
    return; // illegal
  }
  if(LeftDirection == FORWARD){
    Left_DutyB(LeftPeriod-duty, 1); // PB5 is negative logic duty, PB4 is 1
  }else{
    Left_Duty(LeftPeriod-duty, 1);  // PB4 is negative logic duty, PB5 is 1
  }
 }

