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



#include "MotorControl.h"

//#include "OS.H"


static int SteeringAngle, PowerLeft, PowerRight;
enum Direction MotorDirectionLeft, MotorDirectionRight;
//sema4_Type MutexMotors, MutexServo;

/** Motor_Initialization
 * @brief Initialize motors to start state, centered and stopped/coasting
 * @return success or fail
*/
int Motor_Initialization(void){
	// set globals
	// OS_InitSemaphore(&MutexMotor, 1);
	// OS_InitSemaphore(&MutexServo, 1);
	MotorDirectionLeft = MotorDirectionRight = COAST;
	SteeringAngle = SERVOMID;
	PowerRight = POWERMIN;
	PowerLeft = POWERMIN;
	// initialize ports/hardware
	// Servo
	Servo_Init(SERVOPERIOD, SteeringAngle);
	// DC Motors
	DRV8848_LeftInit(PERIODMOTOR, PowerLeft, MotorDirectionLeft);
	DRV8848_RightInit(PERIODMOTOR, PowerRight, MotorDirectionRight);
	return SUCCESS;

}



/** Motor_Steer
 * @brief Turn steering of Robot
 * @parameter degrees angle to turn (-45: Right, 45: Left, 0: Center)
 * @return success or fail
*/
int Motor_Steer(int8_t degrees){
	//OS_Wait(&MutexServo);
	
	// Edge Cases
	if (degrees > 45)
		degrees = 45;
	if (degrees < -45)
		degrees = -45;
	
	// check if centered
	if (degrees == 0){
		SteeringAngle = SERVOMID;
		
	// Use Linear Equation from Excel Sheet
	} else{
		SteeringAngle = AngleCalculation(degrees);
	}
	
	// set bounds
	if (SteeringAngle > SERVOMAX) SteeringAngle = SERVOMAX;
	if (SteeringAngle < SERVOMIN) SteeringAngle = SERVOMIN;
	
	Servo_Duty(SteeringAngle);
	//OS_Signal(&MutexServo);
	return SUCCESS;
}



/** Motor_Speed
 * @brief Adjust speed of Robot (0 - 99)
 * @parameter degrees angle to turn
 * @return success or fail
*/
int Motor_Speed(uint8_t speed){
	//OS_Wait(&MutexMotors);
	
	// edge case
	if (speed > 99)
		speed = 99;
	
	if (speed == 0){
		PowerLeft = POWERMIN;
		PowerRight = POWERMIN;
		DRV8848_LeftStop();
        DRV8848_RightStop();
		return SUCCESS;
	// use linear equation
	} else{
		PowerLeft = PowerRight = speed * POWERDELTA;
	}
		
	DRV8848_LeftInit(PERIODMOTOR, PowerLeft, MotorDirectionLeft);
	DRV8848_RightInit(PERIODMOTOR, PowerRight, MotorDirectionRight);

	//OS_Signal(&MutexMotors);
	return SUCCESS;
}


/** Motor_Direction
 * @brief Adjust direction of robot (forward, reverse, coast)
 * @parameter direction to turn, use enum (FORWARD, REVERSE, COAST}
 * @return success or fail
*/
int Motor_Direction(enum Direction direction){
	// check if valid direction
	if ((direction != COAST) && (direction != FORWARD) && (direction != BACKWARD))
		return FAIL;
	
	//OS_Wait(&MutexMotors);
	
	MotorDirectionLeft = MotorDirectionRight = direction;
	DRV8848_LeftInit(PERIODMOTOR, PowerLeft, MotorDirectionLeft);
    DRV8848_RightInit(PERIODMOTOR, PowerRight, MotorDirectionRight);
	
	
	
	//OS_Signal(&MutexMotors);
	return SUCCESS;
}




/** Motor_Stop
 * @brief Select which motor to stop
 * @parameter motor to disable, use enum (LEFT, RIGHT, BOTH}
 * @return success or fail
*/
int Motor_Stop(uint8_t motor){
	// check if motor selection valid
	if((motor != LEFT) && (motor != RIGHT) && (motor != BOTH))
			return FAIL;

	//OS_Wait(&MutexMotors);
	if (motor == RIGHT){
		 DRV8848_RightStop();
		PowerRight = POWERMIN;

	}else if (motor == LEFT){
		DRV8848_LeftStop();
		PowerLeft = POWERMIN;

	}else{
		MotorDirectionLeft = MotorDirectionRight = COAST;
		PowerLeft = POWERMIN;
		PowerRight = POWERMIN;
		DRV8848_LeftStop();
        DRV8848_RightStop();
		
	}
		
	//OS_Signal(&MutexMotors);
	return SUCCESS;
}



/** Motor_Speed
 * @brief Adjust speed of Robot (0 - 99)
 * @parameter degrees angle to turn
 * @return success or fail
*/
int Motor_SpeedLeft(uint8_t speed){
//OS_Wait(&MutexMotors);
	
	// edge case
	if (speed > 99)
		speed = 99;
	
	if (speed == 0){
		PowerLeft = POWERMIN;
		DRV8848_LeftStop();
		return SUCCESS;
	// use linear equation
	} else{
		PowerLeft = speed * POWERDELTA;
	}
		
	DRV8848_LeftInit(PERIODMOTOR, PowerLeft, MotorDirectionLeft);
	
  

	//OS_Signal(&MutexMotors);
	return SUCCESS;

}


/** Motor_Speed
 * @brief Adjust speed of Robot (0 - 99)
 * @parameter degrees angle to turn
 * @return success or fail
*/
int Motor_SpeedRight(uint8_t speed){
	//OS_Wait(&MutexMotors);
	
	// edge case
	if (speed > 99)
		speed = 99;
	
	if (speed == 0){
		PowerRight = POWERMIN;
        DRV8848_RightStop();
		return SUCCESS;
	// use linear equation
	} else{
		PowerRight = speed * POWERDELTA;
	}
		
    DRV8848_RightInit(PERIODMOTOR, PowerRight, MotorDirectionRight);

	//OS_Signal(&MutexMotors);
	return SUCCESS;
}

/** Motor_DirectionRight
 * @brief Adjust direction of robot (forward, reverse, coast)
 * @parameter direction to turn, use enum (FORWARD, REVERSE, COAST}
 * @return success or fail
*/
int Motor_DirectionRight(enum Direction direction){
	// check if valid direction
	if ((direction != COAST) && (direction != FORWARD) && (direction != BACKWARD))
		return FAIL;
	
	//OS_Wait(&MutexMotors);
	
	MotorDirectionRight = direction;
    DRV8848_RightInit(PERIODMOTOR, PowerRight, MotorDirectionRight);
	
	//OS_Signal(&MutexMotors);
	return SUCCESS;
}



/** Motor_DirectionLeft
 * @brief Adjust direction of robot (forward, reverse, coast)
 * @parameter direction to turn, use enum (FORWARD, REVERSE, COAST}
 * @return success or fail
*/
int Motor_DirectionLeft(enum Direction direction){
	// check if valid direction
	if ((direction != COAST) && (direction != FORWARD) && (direction != BACKWARD))
		return FAIL;
	
	//OS_Wait(&MutexMotors);
	
	MotorDirectionLeft = direction;
	DRV8848_LeftInit(PERIODMOTOR, PowerLeft, MotorDirectionLeft);	
	
	//OS_Signal(&MutexMotors);
	return SUCCESS;
}



