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


  /** MotorRight_InitA
   * @brief Initialize A+ port for H Bridge Motor Driver (PB6), 80 MHz/64 = 1.25 MHz,  PB7 controls direction,
   * @parameter period period of PWM waveform
   * @parameter duty duty cycle of PWM waveform
   * @parameter direction direction of motor: 0-          1-
   * @date 4/9/2018
   * @author Sikender Ashraf
   */
void MotorRight_InitA(uint16_t period, uint16_t duty, int direction);



/** MotorRight_DutyA
 * @brief Set Duty Cycle of PWM wave(PB6), PB7 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorRight_DutyA(uint16_t duty, int direction);



/** MotorRight_InitB
 * @brief Initialize A- port for H Bridge Motor Driver (PB7), 80 MHz/64 = 1.25 MHz,  PB6 controls direction,
 * @parameter period period of PWM waveform
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-         1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorRight_InitB(uint16_t period, uint16_t duty, int direction);



/** MotorRight_DutyB
 * @brief Set Duty Cycle of PWM wave (PB7), PB6 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorRight_DutyB(uint16_t duty, int direction);



/** MotorLeft_InitA
 * @brief Initialize B+ port for H Bridge Motor Driver (PB4), 80 MHz/64 = 1.25 MHz,  PB5 controls direction,
 * @parameter period period of PWM waveform
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-         1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_InitA(uint16_t period, uint16_t duty, int direction);



/** MotorLeft_DutyA
 * @brief Set Duty Cycle of PWM wave (PB4), PB5 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_DutyA(uint16_t duty, int direction);


/** MotorLeft_InitB
 * @brief Initialize B- port for H Bridge Motor Driver (PB5), 80 MHz/64 = 1.25 MHz,  PB4 controls direction,
 * @parameter period period of PWM waveform
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-         1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_InitB(uint16_t period, uint16_t duty, int direction);



/** MotorLeft_DutyB
 * @brief Set Duty Cycle of PWM wave (PB5), PB4 controls direction
 * @parameter duty duty cycle of PWM waveform
 * @parameter direction direction of motor: 0-          1-
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void MotorLeft_DutyB(uint16_t duty, int direction);
