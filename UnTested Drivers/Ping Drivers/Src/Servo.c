/** @file Servo.h
 * @brief Servor drivers for Steering Robot
    Servo Connectons:
      PD0:  Servo A
      PD1:  Servo B
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
#include "tm4c123gh6pm.h"
#include <stdint.h>

  /** Servo_Initilization
   * @brief Initialize Servo Controlling PWM
   * PWM Clock Rate 80MHZ/64 = 1.25MHZ, period in 800ns units, 2 <= duty <= period-1
   * @parameter period period of PWM of Servo
   * @parameter duty duty cycle of PWM waveform
   * @date 4/9/2018
   * @author Sikender Ashraf
   */
 void Servo_Initilization(uint16_t period, uint16_t duty){
     SYSCTL_RCGCPWM_R |= 0x01;             // activate PWM0
     SYSCTL_RCGCGPIO_R |= 0x08;            // activate port D
     while((SYSCTL_PRGPIO_R&0x08) == 0){};
     GPIO_PORTD_AFSEL_R |= 0x01;           // enable alt funct on PD0
     GPIO_PORTD_PCTL_R &= ~0x0000000F;     // configure PD0 as PWM0
     GPIO_PORTD_PCTL_R |= 0x00000004;      //
     GPIO_PORTD_AMSEL_R &= ~0x01;          // disable analog functionality on PD0
     GPIO_PORTD_DEN_R |= 0x01;             // enable digital I/O on PD0
     SYSCTL_RCC_R = 0x001A0000 | (SYSCTL_RCC_R & (~0x000E0000));       // use PWM divider configure for /64 divider
     PWM0_3_CTL_R = 0;                     // re-loading down-counting mode
     PWM0_3_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down, PD0 goes low on LOAD
                                           // PD0 goes high on CMPA down
     PWM0_3_LOAD_R = period - 1;           // cycles needed to count down to 0
     PWM0_3_CMPA_R = duty - 1;             // count value when output rises
     PWM0_3_CTL_R |= 0x00000001;           // start PWM0
     PWM0_ENABLE_R |= 0x00000040;          // enable PD0/M0PWM3A is PWM6
   }




   /** Servo_DutyCycle
    * @brief Set dutycycle for Servo PWM
    * @parameter duty dutycycle
    * @date 4/9/2018
    * @author Sikender Ashraf
    */
   void Servo_DutyCycle(int duty){
     PWM0_3_CMPA_R = duty - 1;              //count value when output rises

   }


 /** Servo_Turn
  * @brief Turn Steering for Servo (-45 to 45 degree range)
  * @parameter turnDegrees degrees to turn
  * @return new turn direction in degrees
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
 int8_t Servo_Turn(int turnDegrees){
    uint16_t duty;
    int8_t newTurnDirection;
    // check boundary conditions
    if(turnDegrees > 45){
        newTurnDirection = 45;
    } else if(turnDegrees < -45){
        newTurnDirection = -45;
    } else{
      newTurnDirection = turnDegrees;
    }
    // calculate new duty cycle from degrees
    duty = (((newTurnDirection+5+40) * 125) / 9) + 1250;
    Servo_DutyCycle(duty);
    return newTurnDirection;
}
