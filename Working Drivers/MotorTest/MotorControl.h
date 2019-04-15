/**
* @file MotorControl.c
* @brief Drivers for controlling DC Motors and Steering Servo
	 Version 6 hardware (use program main)
	 to go forward on right motor
	 PB7 A+  regular GPIO level high (1)
	 PB6 A-  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
	 to go backward on right motor
	 PB7 A+  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
	 PB6 A-  regular GPIO level high (1)
	 coast on right motor (fast decay)
	 PB7 A+  regular GPIO level low (0)
	 PB6 A-  regular GPIO level low (0)
	 to go forward on left motor
	 PB5 B+  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
	 PB4 B-  regular GPIO level high (1) 
	 to go backward on left motor
	 PB5 B+  regular GPIO level high (1)
	 PB4 B-  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
	 coast on left motor (fast decay)
	 PB5 B+  regular GPIO level low (0)
	 PB4 B-  regular GPIO level low (0)
* Contains ADC with periodic sampling and SW called ADC 
* Uses Timer2A (Pri: 2)
* @author Sikender
* @date 4/11/2019
*/

#ifndef __OS_H
#include <stdint.h>
#include "pwm.h"

#define PERIODMOTOR 10000	// min is 1000
#define PERIOD10MS 12500  // 800ns units
#define POWERMIN 5
#define POWERMAX (PERIODMOTOR-100)
#define POWERDELTA 100


// DO NOT CHANGE PERIOD OR YOU WILL OVERDRIVE SERVO!!!!
#define SERVOPERIOD 25000 // DO NOT CHANGE OR YOU WILL OVERDRIVE SERVO!!!!
// DO NOT CHANGE PERIOD OR YOU WILL OVERDRIVE SERVO!!!!

#define SERVOMAX 2800	// Theoretical max is 3125
#define SERVOMID 1700
#define SERVOMIN 700	// Theoretical min is 625
#define SERVODELTA 100

// Angle Calculation stuff
#define SLOPE -21
#define CONSTANT 1725
#define AngleCalculation(angle)(angle*SLOPE+CONSTANT)

enum motorselection {LEFT, RIGHT, BOTH};

#define SUCCESS 1
#define FAIL	0



/** Motor_Initialization
 * @brief Initialize motors to start state, centered and stopped
 * @return success or fail
*/
int Motor_Initialization(void);





/** Motor_Steer
 * @brief Turn steering of Robot
 * @parameter degrees angle to turn (-45: Right, 45: Left, 0: Center)
 * @return success or fail
*/
int Motor_Steer(int8_t degrees);




/** Motor_Speed
 * @brief Adjust speed of Robot (0 - 99)
 * @parameter degrees angle to turn
 * @return success or fail
*/
int Motor_Speed(uint8_t speed);


/** Motor_Direction
 * @brief Adjust direction of robot (forward, reverse, coast)
 * @parameter direction to turn, use enum (FORWARD, REVERSE, COAST}
 * @return success or fail
*/
int Motor_Direction(enum Direction direction);




/** Motor_Stop
 * @brief Select which motor to stop
 * @parameter motor to disable, use enum (LEFT, RIGHT, BOTH}
 * @return success or fail
*/
int Motor_Stop(uint8_t motor);



/** Motor_Speed
 * @brief Adjust speed of Robot (0 - 99)
 * @parameter degrees angle to turn
 * @return success or fail
*/
int Motor_SpeedLeft(uint8_t speed);


/** Motor_Speed
 * @brief Adjust speed of Robot (0 - 99)
 * @parameter degrees angle to turn
 * @return success or fail
*/
int Motor_SpeedRight(uint8_t speed);



/** Motor_DirectionRight
 * @brief Adjust direction of robot (forward, reverse, coast)
 * @parameter direction to turn, use enum (FORWARD, REVERSE, COAST}
 * @return success or fail
*/
int Motor_DirectionRight(enum Direction direction);


/** Motor_DirectionLeft
 * @brief Adjust direction of robot (forward, reverse, coast)
 * @parameter direction to turn, use enum (FORWARD, REVERSE, COAST}
 * @return success or fail
*/
int Motor_DirectionLeft(enum Direction direction);









#endif





