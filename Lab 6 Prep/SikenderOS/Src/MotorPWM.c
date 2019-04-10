/** @file MotorPWM.c
 * @brief Drivers for Motor of Robot
    Motor Connectons:
      PB7 A+  PWM 100Hz   /right motor/PWM positive logic
      PB6 A-  normal GPIO/right motor/0 means forward
      PB5 B+  PWM 100Hz   /left motor /PWM negative logic
      PB4 B-  normal GPIO/left motor /1 means forward
 * @date 4/9/2018
 * @author Sikender Ashraf
 */

#include "tm4c123gh6pm.h"

 /** MotorRight_InitA
  * @brief Initialize A+ port for H Bridge Motor Driver (PB6), 80 MHz/64 = 1.25 MHz,  PB7 controls direction,
  * @parameter period period of PWM waveform
  * @parameter duty duty cycle of PWM waveform
  * @parameter direction direction of motor: 0-          1-
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void MotorRight_InitA(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on PB6
  GPIO_PORTB_AFSEL_R &= ~0x80;          // disable alt funct on PB7
  GPIO_PORTB_PCTL_R &= ~0xFF000000;     // configure PB6 as PWM0
  GPIO_PORTB_PCTL_R |= 0x04000000;      // PB7 regular
  GPIO_PORTB_AMSEL_R &= ~0xC0;          // disable analog functionality on PB6
  GPIO_PORTB_DIR_R |= 0xC0;             // PB7, PB6 output
  GPIO_PORTB_DEN_R |= 0xC0;             // enable digital I/O on PB6
  SYSCTL_RCC_R = 0x001A0000 |           // use PWM divider
      (SYSCTL_RCC_R & (~0x000E0000));   // configure for /64 divider
  PWM0_0_CTL_R = 0;                     // re-loading down-counting mode
  PWM0_0_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PB6 goes low on LOAD
  // PB6 goes high on CMPA down
  PWM0_0_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_0_CMPA_R = duty - 1;             // count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // start PWM0
  PWM0_ENABLE_R &= ~0x00000002;         // disable PB7/M0PWM1
  PWM0_ENABLE_R |= 0x00000001;          // enable PB6/M0PWM0
  // set direction
  if(direction){
    PB7 = 0x80;
  }else{
    PB7 = 0;
  }
}



/** MotorRight_DutyA
 * @brief Set Duty Cycle of PWM wave(PB6), PB7 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorRight_DutyA(uint16_t duty, int direction){
  PWM0_0_CMPA_R = duty - 1;             // count value when output rises
  if(direction){
    PB7 = 0x80;
  }else{
    PB7 = 0;
  }
}



/** MotorRight_InitB
 * @brief Initialize A- port for H Bridge Motor Driver (PB7), 80 MHz/64 = 1.25 MHz,  PB6 controls direction,
 * @parameter period period of PWM waveform
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-         1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorRight_InitB(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x80;           // enable alt funct on PB7
  GPIO_PORTB_AFSEL_R &= ~0x40;          // disable alt funct on PB6
  GPIO_PORTB_PCTL_R &= ~0xFF000000;     // configure PB7 as PWM0B = M0PWM1
  GPIO_PORTB_PCTL_R |= 0x40000000;      // PB6 regular
  GPIO_PORTB_AMSEL_R &= ~0xC0;          // disable analog functionality on PB7,PB6
  GPIO_PORTB_DIR_R |= 0xC0;             // PB6 PB7 output
  GPIO_PORTB_DEN_R |= 0xC0;             // enable digital I/O on PB7,PB6
  SYSCTL_RCC_R = 0x001A0000 |           // use PWM divider
      (SYSCTL_RCC_R & (~0x000E0000));   // configure for /64 divider
  PWM0_0_CTL_R = 0;                     // re-loading down-counting mode
  PWM0_0_GENB_R = 0xC08;                // low on LOAD, high on CMPB down
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM0_0_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_0_CMPB_R = duty - 1;             // count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // start PWM0
  PWM0_ENABLE_R &= ~0x00000001;         // disable PB6/M0PWM0
  PWM0_ENABLE_R |= 0x00000002;          // enable PB7/M0PWM1
  if(direction){
    PB6 = 0x40;
  }else{
    PB6 = 0;
  }
}


/** MotorRight_DutyB
 * @brief Set Duty Cycle of PWM wave (PB7), PB6 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorRight_DutyB(uint16_t duty, int direction){
  PWM0_0_CMPB_R = duty - 1;             // count value when output rises
  if(direction){
    PB6 = 0x40;
  }else{
    PB6 = 0;
  }
}


/** MotorLeft_InitA
 * @brief Initialize B+ port for H Bridge Motor Driver (PB4), 80 MHz/64 = 1.25 MHz,  PB5 controls direction,
 * @parameter period period of PWM waveform
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-         1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_InitA(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x10;           // enable alt funct on PB4
  GPIO_PORTB_AFSEL_R &= ~0x20;          // disable alt funct on PB5
  GPIO_PORTB_PCTL_R &= ~0x00FF0000;     // configure PB4 as PWM0
  GPIO_PORTB_PCTL_R |= 0x00040000;      // PB5 regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x30;          // disable analog functionality on PB4,5
  GPIO_PORTB_DIR_R |= 0x30;             // PB5 PB4 outputs
  GPIO_PORTB_DEN_R |= 0x30;             // enable digital I/O on PB4, PB5
  SYSCTL_RCC_R = 0x001A0000 |           // use PWM divider
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_1_CTL_R = 0;                     // re-loading down-counting mode
  PWM0_1_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PB4 goes low on LOAD
  // PB4 goes high on CMPA down
  PWM0_1_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_1_CMPA_R = duty - 1;             // count value when output rises
  PWM0_1_CTL_R |= 0x00000001;           // start PWM0
  PWM0_ENABLE_R &= ~0x00000008;         // disable PB5/M0PWM1B
  PWM0_ENABLE_R |= 0x00000004;          // enable PB4/M0PWM1A is PWM2
  if(direction){
    PB5 = 0x20;
  }else{
    PB5 = 0;
  }
}



/** MotorLeft_DutyA
 * @brief Set Duty Cycle of PWM wave (PB4), PB5 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_DutyA(uint16_t duty, int direction){
  PWM0_1_CMPA_R = duty - 1;    // count value when output rises
  if(direction){
    PB5 = 0x20;
  }else{
    PB5 = 0;
  }
}

/** MotorLeft_InitB
 * @brief Initialize B- port for H Bridge Motor Driver (PB5), 80 MHz/64 = 1.25 MHz,  PB4 controls direction,
 * @parameter period period of PWM waveform
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-         1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_InitB(uint16_t period, uint16_t duty, int direction){
  SYSCTL_RCGCPWM_R |= 0x01;             // activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x20;           // enable alt funct on PB5
  GPIO_PORTB_AFSEL_R &= ~0x10;          // disable alt funct on PB4
  GPIO_PORTB_PCTL_R &= ~0x00FF0000;     // configure PB5 as PWM3
  GPIO_PORTB_PCTL_R |= 0x00400000;      // PB4 regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x30;          // disable analog functionality on PB4,5
  GPIO_PORTB_DIR_R |= 0x30;             // PB5 PB4 outputs
  GPIO_PORTB_DEN_R |= 0x30;             // enable digital I/O on PB4, PB5
  SYSCTL_RCC_R = 0x001A0000 |           // use PWM divider
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /64 divider
  PWM0_1_CTL_R = 0;                     // re-loading down-counting mode
  PWM0_1_GENB_R = 0xC08;                // low on LOAD, high on CMPB down
  // PB5 goes low on LOAD
  // PB45 goes high on CMPB down
  PWM0_1_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_1_CMPB_R = duty - 1;             // count value when output rises
  PWM0_1_CTL_R |= 0x00000001;           // start PWM0
  PWM0_ENABLE_R &= ~0x00000004;         // disable PB4/M0PWM1A
  PWM0_ENABLE_R |= 0x00000008;          // enable PB5/M0PWM1B is PWM3
  if(direction){
    PB4 = 0x10;
  }else{
    PB4 = 0;
  }
}




/** MotorLeft_DutyB
 * @brief Set Duty Cycle of PWM wave (PB5), PB4 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_DutyB(uint16_t duty, int direction){
  PWM0_1_CMPB_R = duty - 1;    // count value when output rises
    if(direction){
    PB4 = 0x10;
  }else{
    PB4 = 0;
  }
  }
