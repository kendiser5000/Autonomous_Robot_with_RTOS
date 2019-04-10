

#include "MotorPWM.h"







int main(void){ // version 6 with DRV8848
  int direction = 0;   // forward
  PLL_Init(Bus80MHz);  // bus clock at 80 MHz
  PortF_Init();
  Power = POWERMIN;
  Steering = SERVOMID;  // 20ms period 1.5ms pulse
  SteeringMode = 0;
  Servo_Init(25000, Steering);
  DRV8848_LeftInit(PERIOD10MS, Power, COAST);
  DRV8848_RightInit(PERIOD10MS, Power, COAST);
  while(1){
    WaitForTouch();
    if(PF0==0){
      if(SteeringMode){
        if(Steering >= SERVOMIN+SERVODELTA){
          Steering = Steering - SERVODELTA;
        }else{
          Steering = SERVOMID; // go to center and
          SteeringMode = 0;    // switch direction
        }
      }else{
        Steering = Steering + SERVODELTA;
        if(Steering > SERVOMAX){
          Steering = SERVOMID; // go to center and
          SteeringMode = 1;    // switch direction
        }
      }
      Servo_Duty(Steering);    // SERVOMIN to SERVOMAX
      PF1 ^= 0x02;
    }
    if((PF0==0x01)&&(PF4==0)){
      Power = Power + POWERDELTA;
      if(Power > POWERMAX){
        Power = POWERMIN;      // go back to minimum
      }
      PF2 ^= 0x04;
      if(Power == POWERMIN){
        DRV8848_LeftStop();
        DRV8848_RightStop();}
      else if(Power == (POWERMIN+POWERDELTA)){
        if(direction){
          DRV8848_LeftInit(PERIOD10MS, Power, BACKWARD);
          DRV8848_RightInit(PERIOD10MS, Power, BACKWARD);
          direction = 0;
        }else{
          DRV8848_LeftInit(PERIOD10MS, Power, FORWARD);
          DRV8848_RightInit(PERIOD10MS, Power, FORWARD);
          direction = 1;
        }
      }else{
        DRV8848_LeftDuty(Power);   // 400 to 12400 (positive logic)
        DRV8848_RightDuty(Power);  // 400 to 12400 (negative logic)
      }
    }
    WaitForRelease();
  }
}
