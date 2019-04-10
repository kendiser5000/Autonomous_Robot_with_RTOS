/** @file Servo.h
 * @brief Servor drivers for Steering Robot
    Servo Connectons:
      PD0:  Servo A
      PD1:  Servo B
 * @date 4/9/2018
 * @author Sikender Ashraf
 */



 /** Servo_Initilization
  * @brief Initialize Servo Controlling PWM
  * @parameter period period of PWM of Servo
  * @parameter duty duty cycle of PWM waveform
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Servo_Initilization(uint16_t period, uint16_t duty);




/** Servo_Turn
 * @brief Turn Steering for Servo (-45 to 45 degree range)
 * @parameter turnDegrees degrees to turn
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Servo_Turn(int turnDegrees);



/** Servo_DutyCycle
 * @brief Set dutycycle for Servo PWM
 * @parameter duty dutycycle
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Servo_DutyCycle(int duty);
