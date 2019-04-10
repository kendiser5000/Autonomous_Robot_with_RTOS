/** @file MotorPWM.h
 * @brief Drivers for Motor of Robot
    Motor Connectons:
      PB7 A+  PWM 100Hz   /right motor/PWM positive logic
      PB6 A-  normal GPIO/right motor/0 means forward
      PB5 B+  PWM 100Hz   /left motor /PWM negative logic
      PB4 B-  normal GPIO/left motor /1 means forward
 * @date 4/9/2018
 * @author Sikender Ashraf
 */


void MotorLeft_Init(uint16_t period, uint16_t duty);


void MotorRight_Init(uint16_t period, uint16_t duty);
