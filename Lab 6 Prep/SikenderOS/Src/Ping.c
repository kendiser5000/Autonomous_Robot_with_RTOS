/** @file Ping.c
 * @brief Drivers for Ping Sensor
    PING PORTB CONNECTIONS:    
	 HC-SR04 Ultrasonic Range Finder 
	 J9X  Trigger0 to PB7 output (10us pulse)
	 J9X  Echo0    to PB6 T0CCP0
	 J10X Trigger1 to PB5 output (10us pulse)
	 J10X Echo1    to PB4 T1CCP0
	 J11X Trigger2 to PB3 output (10us pulse)
	 J11X Echo2    to PB2 T3CCP0
	 J12X Trigger3 to PC5 output (10us pulse)
	 J12X Echo3    to PF4 T2CCP0
	 J9Y  Trigger/Echo0 to PB6 T0CCP0
	 J10Y Trigger/Echo1 to PB4 T1CCP0
	 J11Y Trigger/Echo2 to PB2 T3CCP0
	 J12Y Trigger/Echo3 to PF4 T2CCP0
	 IR distance sensors:
		 J5/A0/PE3
		 J6/A1/PE2
		 J7/A2/PE1
		 J8/A3/PE0  
 * @date 4/9/2018
 * @author Sikender Ashraf
 */

#include "Ping.h"
#include "tm4c123gh6pm.h"
#include "Filter.h"
#include "OS.h"


/* timer3: Trigger PB7, Measure/echo PB6 */
#define TRIGGER3    (*((volatile uint32_t *)0x40005020))//
#define TRIGGER3HIGH 0x08//
#define ECHO3       (*((volatile uint32_t *)0x40005010))//
#define ECHO3HIGH 0x04//
int Timer3First,Timer3Done,Timer3Pulse;

/* timer1: Trigger PB5, Measure/echo PB4 */
#define TRIGGER1    (*((volatile uint32_t *)0x40005080))//
#define TRIGGER1HIGH 0x20//
#define ECHO1       (*((volatile uint32_t *)0x40005040))//
#define ECHO1HIGH 0x10//
int Timer1First, Timer1Done, Timer1Pulse;

/* timer0: Trigger PB7, Measure/echo PB6 */
#define TRIGGER0    (*((volatile uint32_t *)0x40005200))//
#define TRIGGER0HIGH 0x80 //
#define ECHO0       (*((volatile uint32_t *)0x40005100))//
#define ECHO0HIGH 0x40//
int Timer0First, Timer0Done, Timer0Pulse;


/* Median Filter for Ultrasonic*/
AddFilter(USonicSensor);
/* USonic DataRdy, semaphore for data ready */
Sema4Type USonicDataRdy;

// private global to track usonic values
int static Values[NUM_USONIC];


/** DelayWait10us
  * @brief BusyWait for 10 us
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void DelayWait10us(void){uint32_t volatile time;
  time = 800/12;  // 10usec
  while(time){
    time--; // 12 cycles
  }
}

/** DelayWait5us
  * @brief BusyWait for 5 us
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void DelayWait5us(void){uint32_t volatile time;
  time = 400/12;  // 5usec
  while(time){
    time--; // 12 cycles
  }
}



/** Timer0_init
  * @brief Timer0 for edge time mode to request interrupt on both edges of PB6 (TOCCP0), PB7 is output
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer0_Init(void){
	Timer0First = Timer0Done = Timer0Pulse = 0;
	volatile int delay;
	SYSCTL_RCGCTIMER_R |= 0x01;		// Enable Timer0
	SYSCTL_RCGCGPIO_R |= 0x02; 		// Enable PortB
	while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?
	//delay = 42;
	GPIO_PORTB_DIR_R |= 0x80;        	// PB7 is output
	GPIO_PORTB_DIR_R &= ~0x40;       	// PB6 is input
	GPIO_PORTB_AFSEL_R |= 0x40;      	// Alt Funct PB6
	GPIO_PORTB_AFSEL_R &= ~0x80;     	// No Alt Funct PB6
	GPIO_PORTB_DEN_R |= 0xC0;        	// PB6 is input for T0CCP0
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0x00FFFFFF)+0x07000000;
	GPIO_PORTB_AMSEL_R &= ~0xC0;     	// disable analog functionality
	//**************************
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN; 	// disable timer0A during setup
	TIMER0_CFG_R = TIMER_CFG_16_BIT; 	// 16-bit timer mode
	TIMER0_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);   // 24bit timer capture         
	TIMER0_CTL_R |= TIMER_CTL_TAEVENT_BOTH;// Double Edge
	TIMER0_TAILR_R = TIMER_TAILR_M;  // Max STart Value
	TIMER0_TAPR_R = 0xFF;            // Prescale for 24 bit
	TIMER0_IMR_R |= TIMER_IMR_CAEIM; // Input Capture match interrupt
	TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
	TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupt
	// int stuff
	NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits, Pri = 2
	NVIC_EN0_R = 1<<19;        // enable interrupt 19 in NVIC
}



/** Timer0_StartHCSRO4
  * @brief Start HCSR04 ultrasonic dist measure, 10us output on PB7
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer0_StartHCSR04(void){
	Timer0Done = 0;
	TRIGGER0 = TRIGGER0HIGH;	// set output as high
	// wait
	DelayWait10us();
	DelayWait10us();
	// read pulse
	TRIGGER0 = 0;
	TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
}



/** Timer0_StartPing
  * @brief Start ultrasonic dist measure, PB7 is output, PB6 measures pulse of input (timer capture)
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer0_StartPing(void){
	GPIO_PORTB_AFSEL_R &= ~0x40;     	// disable alt funct on PB6
	GPIO_PORTB_PCTL_R = GPIO_PORTB_PCTL_R&0xF0FFFFFF;
	GPIO_PORTB_DIR_R |= 0x40;        	// make PB6 output
	Timer0Done = 0;//
	ECHO0 = ECHO0HIGH;				// set output to high
	// wait
	DelayWait5us();
	// read pulse
	ECHO0 = 0;						// disable output
	GPIO_PORTB_DIR_R &= ~0x40;       	// make PB6 in
	GPIO_PORTB_AFSEL_R |= 0x40;      	// enable alt funct on PB6
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
	TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
}




/** Timer0_Read
  * @brief Read pulse measurement
  * @return 0 if not ready, else get cycle measurement
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
uint32_t Timer0_Read(void){
  if(Timer0Done){
    return Timer0Pulse;
  }
  return 0;
}



/** Timer0_Handler
  * @brief Handler for Timer0A, finished pulse
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer0A_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_CAECINT;		// acknowledge flag
	//
	if(ECHO0 == ECHO0HIGH){ 				// check if starting up
		Timer0First = TIMER0_TAR_R;
	}else{
		Values[0] = (Timer0First - TIMER0_TAR_R)&0xFFFFFF;	// only 24 bits needed
		Timer0Done = 1;
		  // use flags to determine if done
			#ifdef USE3USONIC
				if(Timer1Done && Timer3Done){
					OS_bSignal(&USonicDataRdy);
				}
			#endif
			#ifdef USE2USONIC
				if(Timer1Done){
					OS_bSignal(&USonicDataRdy);
				}
			#endif
			#ifdef USE1USONIC
				OS_bSignal(&USonicDataRdy);
			#endif
	}
}


//*************************************************************************************************************8
#ifndef USE1USONIC

/** Timer1_Init
  * @brief Init Timer 1A as input capture for double edge of PB4 (T1CCPO), PB5 output
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer1_Init(void){
	Timer1First = Timer1Done = Timer1Pulse = 0;
	SYSCTL_RCGCTIMER_R |= 0x02;			// Enable timer
	SYSCTL_RCGCGPIO_R |= 0x02; 			// Enable PortB
	// delay
	while((SYSCTL_PRGPIO_R&0x0002) == 0){};
		//
	GPIO_PORTB_DIR_R |= 0x20;        // PB5 is output
	GPIO_PORTB_DIR_R &= ~0x10;       // PB4 is input
	GPIO_PORTB_AFSEL_R |= 0x10;      // beep boop bits and bytes
	GPIO_PORTB_AFSEL_R &= ~0x20;     // beep boop bits and bytes
	GPIO_PORTB_DEN_R |= 0x30;        // enable PB4 as T1CCP0
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF00FFFF)+0x00070000;
	GPIO_PORTB_AMSEL_R &= ~0x30;     // Disable Analog functionality
	TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // Disable Timer
	TIMER1_CFG_R = TIMER_CFG_16_BIT; // 16 bit mode
	TIMER1_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);   // 24 bit capture mode       
	TIMER1_CTL_R |= TIMER_CTL_TAEVENT_BOTH;	// double edge triggered
	TIMER1_TAPR_R = 0xFF;            // 24 bit prescale
	TIMER1_TAILR_R = TIMER_TAILR_M;  // max value
	TIMER1_IMR_R |= TIMER_IMR_CAEIM; // Enable input capture
	TIMER1_ICR_R = TIMER_ICR_CAECINT;// ack flag
	TIMER1_CTL_R |= TIMER_CTL_TAEN;  // Enable timer
							
	NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000; // bits 15-13, pri = 2
	NVIC_EN0_R = 1<<21;           // enable IRQ 21 in NVIC
}


/** Timer1_StartHCSR04
  * @brief Start dist measureument, 10us pulse delays
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer1_StartHCSR04(void){
	Timer1Done = 0;
	// ping for signal
	TRIGGER1 = TRIGGER1HIGH;
	DelayWait10us();
	DelayWait10us();
	TRIGGER1 = 0;
	// start input capture
	TIMER1_ICR_R = TIMER_ICR_CAECINT;
}


/** Timer1_StartPing
  * @brief Init PB4 & PB5, send pulse, get input response
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer1_StartPing(void){
	GPIO_PORTB_AFSEL_R &= ~0x10;     
	GPIO_PORTB_PCTL_R = GPIO_PORTB_PCTL_R&0xFFF0FFFF;
	GPIO_PORTB_DIR_R |= 0x10;    
	// send signal and delay
	Timer1Done = 0;
	ECHO1 = ECHO1HIGH;	// set output high
	DelayWait5us();
	ECHO1 = 0;
	//
	GPIO_PORTB_DIR_R &= ~0x10;       
	GPIO_PORTB_AFSEL_R |= 0x10;      
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFF0FFFF)+0x00070000;
	// start input capture time
	TIMER1_ICR_R = TIMER_ICR_CAECINT;
}



/** Timer1_Read
  * @brief Read Timer1A pulse
  * @return 0 if not ready, else get cycle measurement
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
uint32_t Timer1_Read(void){
  if(Timer1Done){
    return Timer1Pulse;
  }
  return 0;
}


/** Timer1A_Handler
  * @brief Handler for input capture of pulse, gather input data
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_CAECINT;	//ack flag
	// do capturing, dont count first time (eee)
	if(ECHO1 == ECHO1HIGH){
		Timer1First = TIMER1_TAR_R;
	}else{
		Values[1] = (Timer1First - TIMER1_TAR_R)&0xFFFFFF;
		Timer1Done = 1;
			// condition for Usoncis
			#ifdef USE3USONIC
				if(Timer0Done && Timer3Done){
					OS_bSignal(&USonicDataRdy);
				}
			#endif
			#ifdef USE2USONIC
				if(Timer0Done){
					OS_bSignal(&USonicDataRdy);
				}
			#endif
  }
}
#endif




//***********************************************************************************************************8
#ifdef USE3USONIC

/** Timer3_Init
  * @brief Init Timer 3A as input capture for double edge of PB2 (T3CCP0), PB3 output
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer3_Init(void){
	Timer3First = Timer3Done = Timer3Pulse = 0;
	SYSCTL_RCGCTIMER_R |= 0x08;				// Enable timer
	SYSCTL_RCGCGPIO_R |= 0x02; 				// Enable PortB
	//bw
	while((SYSCTL_PRGPIO_R&0x02) == 0){};
		//
	GPIO_PORTB_DIR_R |= 0x08;        // PB3 is output
	GPIO_PORTB_DIR_R &= ~0x04;       // PB2 is input
	GPIO_PORTB_AFSEL_R |= 0x04;      // beep boop
	GPIO_PORTB_AFSEL_R &= ~0x08;     // bit and bytes make you have an appetitite
	GPIO_PORTB_DEN_R |= 0x0C;        
		//alt function for input capture
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00000700;
	GPIO_PORTB_AMSEL_R &= ~0x0C;     // Disable analog func
	TIMER3_CTL_R &= ~TIMER_CTL_TAEN; // Disable Timer
	TIMER3_CFG_R = TIMER_CFG_16_BIT; // 16 bit mode
	TIMER3_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);   // capture 24 bit    
	TIMER3_CTL_R |= TIMER_CTL_TAEVENT_BOTH;		// double edge trigger
	TIMER3_TAILR_R = TIMER_TAILR_M;  // (Brandie) maxx value
	TIMER3_TAPR_R = 0xFF;            // prescale 24 it
	TIMER3_IMR_R |= TIMER_IMR_CAEIM; // Enable input capture
	TIMER3_ICR_R = TIMER_ICR_CAECINT;	// set flag
	TIMER3_CTL_R |= TIMER_CTL_TAEN;  // start timer
	NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // pri = 2
	
	// vector number 51, interrupt number 35
	NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
}


/** Timer3_StartHCSR04
  * @brief Start Dist measurement, 10us pulse
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer3_StartHCSR04(void){
	Timer3Done = 0;
	// set high
	TRIGGER3 = TRIGGER3HIGH;
	DelayWait10us();
	DelayWait10us();
	TRIGGER3 = 0;
	// start timer
	TIMER3_ICR_R = TIMER_ICR_CAECINT;// clear timer3A capture match flag
}



/** Timer3_StartPing
  * @brief Ping sensor for dist measurement
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer3_StartPing(void){
	GPIO_PORTB_AFSEL_R &= ~0x04;//     
	GPIO_PORTB_PCTL_R = GPIO_PORTB_PCTL_R&0xFFFFF0FF;
	GPIO_PORTB_DIR_R |= 0x04;//
	Timer3Done = 0;
	// do pinging, this sensor is trash
	ECHO3 = ECHO3HIGH;
	DelayWait5us();
	ECHO3 = 0;
	// read pulse
	GPIO_PORTB_DIR_R &= ~0x04;       
	GPIO_PORTB_AFSEL_R |= 0x04;    
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFF0FF)+0x00000700;
	TIMER3_ICR_R = TIMER_ICR_CAECINT;
}


//------------Timer3_Read------------
// read ultrasonic distance measurement
// Input: none
// Output: 0 if not ready, pulse width in 12.5ns time if ready
/** Timer3_Read
  * @brief read distance
  * @return time if ready, else 0 if not ready
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
uint32_t Timer3_Read(void){
	// check if done
	if(Timer3Done){
		return Timer3Pulse;
	} else{
		return 0;}
}


/** Timer3A_Handler
  * @brief Do the input captur my dude
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Timer3A_Handler(void){
	TIMER3_ICR_R = TIMER_ICR_CAECINT;
	// ski[ first edge
	if(ECHO3 == ECHO3HIGH){ 
	Timer3First = TIMER3_TAR_R;
	}else{
		Values[2] = (Timer3First - TIMER3_TAR_R)&0xFFFFFF;
		Timer3Done = 1;
			#ifdef USE3USONIC
				if(Timer1Done && Timer0Done){
					OS_bSignal(&USonicDataRdy);
				}
			#endif
  }
}
#endif

void Ping_Init(void){
	OS_InitSemaphore(&USonicDataRdy, 0);
	#ifdef USE3USONIC
		Timer0_Init();
		Timer1_Init();
		Timer3_Init();
	#endif
	#ifdef USE2USONIC
		Timer0_Init();
		Timer1_Init();
	#endif
	#ifdef USE1USONIC
		Timer0_Init();
	#endif
}

void Ping_StartHCSR04(void){
	#ifdef USE3USONIC
		Timer0_StartHCSR04();
		Timer1_StartHCSR04();
		Timer3_StartHCSR04();
	#endif
	#ifdef USE2USONIC
		Timer0_StartHCSR04();
		Timer1_StartHCSR04();
	#endif
	#ifdef USE1USONIC
		Timer0_StartHCSR04();
	#endif
}
void Ping_StartPing(void){
	#ifdef USE3USONIC
		Timer0_StartPing();
		Timer1_StartPing();
		Timer3_StartPing();
	#endif
	#ifdef USE2USONIC
		Timer0_StartPing();
		Timer1_StartPing();
	#endif
	#ifdef USE1USONIC
		Timer0_StartPing();
	#endif
}

#define BUS 80000

/** Cycles2milliInch
  * @brief Convert cycles to distance of .001 inches, // (x cycles)*(1 sec/80,000,000 cycles)*(13,560 in/sec)*(1,000 mil/1 in)*(0.5 round trip)
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
uint32_t Cycles2milliInch(uint32_t cycles){
	return (cycles*6780)/BUS; 
}

/** Cycles2millimeter
  * @brief Convert cycles to distance of millimeter, // (x cycles)*(1 sec/80,000,000 cycles)*(340.29 m/sec)*(1,000 mm/1 m)*(0.5 round trip)
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
uint32_t Cycles2millimeter(uint32_t cycles){
	return (cycles*170)/BUS;  
}


/** Ping_GetData
  * @brief Get Distance of Ping sesor in mm
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Ping_GetData(uint32_t data[NUM_USONIC]){
	// wait till ready
	OS_Wait(&USonicDataRdy);
	data[0] = FilterUSonicSensor(Cycles2millimeter(Values[0]));
}
