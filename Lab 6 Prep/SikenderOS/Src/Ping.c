/** @file Ping.c
 * @brief Drivers for Ping Sensor
    PING PORTB CONNECTIONS:
      Gnd: GND
      5V: 5V
      Signal: PB6
 * @date 4/9/2018
 * @author Sikender Ashraf
 */


#include "OS.h"
#include "tm4c123gh6pm.h"

#define PORTB_PORTS 0x40
#define PORTB_PING_PRIORITY 1

int StartCritical(void);
void EndCritical(int sr);

 /*
PING PORTB CONNECTIONS:
Gnd: GND
5V: 5V
Signal: PB6
*/





/** A void Function Type
 * Task to run upon ping detection
 */
void (*PING_PORTB_PeriodicTask)(void);


/** Time taken for ping
 * Task to run upon ping detection
 */
unsigned long PING_PORTB_Time;

 /** Ping_PORTB_Initilization
  * @brief Initialize PortB for ping sensor (without Timer 1)
  * @parameter task task to run when sensor sends signal
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Ping_PORTB_Initilization(void(*task) (void)){
    int sr = StartCritical();
    SYSCTL_RCGCGPIO_R |= 0x2;             // activate port B
    PING_PORTB_PeriodicTask = task;       // user function
    GPIO_PORTB_DIR_R |= PORTB_PORTS;    	// PB6 out
    GPIO_PORTB_AFSEL_R &= ~PORTB_PORTS;  	// disable alt funct on PB64
    GPIO_PORTB_DEN_R |= PORTB_PORTS;     	// enable digital I/O on PB6
    																			// configure PF4 as GPIO
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x00000000;
    GPIO_PORTB_AMSEL_R = 0;       				// disable analog functionality on PB
    //GPIO_PORTB_PUR_R |= PORTB_PORTS;    // enable weak pull-up on PB6
    GPIO_PORTB_PDR_R |= PORTB_PORTS;    	// enable weak pull-down on PB6
    GPIO_PORTB_IS_R &= ~PORTB_PORTS;     	// PB64 is edge-sensitive
    	//GPIO_PORTB_IBE_R &= ~PORTB_PORTS; // PB6 is not both edges
    GPIO_PORTB_IBE_R |= PORTB_PORTS;    	// PB6 is both edges
    	//GPIO_PORTB_IEV_R &= ~PORTB_PORTS; //     PB6 falling edge event
    GPIO_PORTB_ICR_R |= PORTB_PORTS;      // clear flag6
    GPIO_PORTB_IM_R |= PORTB_PORTS;       // arm interrupt on PB6
                                          // interrupt enable
    NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF0FFF)|0x00004000;
    NVIC_EN0_R = NVIC_PRI0_INT1_M;     // enable interrupt 1 in NVIC
    EndCritical(sr);
}





/** Ping_PORTB_Initilization
 * @brief Initialize Ping sensor ports
 * @parameter task task to run when sensor sends signal
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
  void Ping_Initilizations(void(*task)(void)){
    Ping_PORTB_Initilization(task);
  }


  /** Ping_PORTB_Signal
   * @brief Starts contact with sensor
   * @parameter task task to run when sensor sends signal
   * @date 4/9/2018
   * @author Sikender Ashraf
   */
void Ping_PORTB_Signal(void){
    int sr;
    sr = StartCritical();	//diable interrupts
    GPIO_PORTB_IM_R &= ~PORTB_PORTS;
    GPIO_PORTB_DIR_R |= PORTB_PORTS; //make PB6 out
    GPIO_PORTB_DATA_R |= PORTB_PORTS; //set PB6 high
    // small delay
    for(volatile int i = 0; i < 100; i++){}

    GPIO_PORTB_DATA_R &= ~PORTB_PORTS; //set PB6 low
    GPIO_PORTB_DIR_R &= ~PORTB_PORTS; //make PB6 in
    GPIO_PORTB_DATA_R &= ~PORTB_PORTS; //set PB6 low
    GPIO_PORTB_ICR_R = PORTB_PORTS;      // acknowledge flag6
    GPIO_PORTB_IM_R |= PORTB_PORTS;
    EndCritical(sr);	//enable interrupts
    OS_Kill();

}


/** Ping_PORTB_Start
 * @brief Start Signal for Ping Sensor
 * @parameter task task to run when sensor sends signal
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Ping_PORTB_Start(void){
    OS_AddThread(&Ping_PORTB_Signal, 128, PORTB_PING_PRIORITY);	// should this be Ping_PORTB_Signal
}

unsigned long Ping_Value(void){
	return PING_PORTB_Time;
}

/** Ping_PORTB_Signal
 * @brief Starts contact with sensor
 * @parameter task task to run when sensor sends signal
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void GPIOPortB_Handler(void){
    static unsigned long startTime, data, time;
    time = OS_Time();
    data = GPIO_PORTB_DATA_R;
    //PB6 high
    if((data & 0x40) == 0x40){
        startTime = time;
    }
     //PB6 low
    else if((data & 0x40) == 0){
    		PING_PORTB_Time = OS_TimeDifference(startTime, time);
    		OS_AddThread(PING_PORTB_PeriodicTask,128, PORTB_PING_PRIORITY);
    }
    GPIO_PORTB_ICR_R = PORTB_PORTS;      // acknowledge flag6
    }
