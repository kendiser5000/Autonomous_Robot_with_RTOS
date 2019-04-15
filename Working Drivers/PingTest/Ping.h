/** @file Ping.c
 * @brief Drivers for Ping Sensor
    PING PORTB CONNECTIONS:
      Gnd: GND
      5V: 5V
      Signal: PB6
 * @date 4/9/2018
 * @author Sikender Ashraf
 */

 #ifndef __Ping_H__
 #define __Ping_H__
#include <stdint.h>



/* declare number of sensors to use */
#define NUMPINGSENSOR 3

// one for testing
/*
#define USE1USONIC 1
#define USE2USONIC 1
*/


#define USE3USONIC 1





#ifdef USE1USONIC
	#define NUM_USONIC 1
#endif

#ifdef USE2USONIC
	#define NUM_USONIC 2
#endif

#ifdef USE3USONIC
	#define NUM_USONIC 3
#endif


 /** DelayWait10us
  * @brief BusyWait for 10 us
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void DelayWait10us(void);

 /** Ping_Init
  * @brief Initialize Ping sensor ports
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Ping_Init(void);


/** Ping_StartHCR04
 * @brief Start distance measurement
 * 10us pulse output in GPIO
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Ping_StartHCSR04(void);



/** Ping_StartPing
 * @brief Start distance measurement
 * Send Pulse and capture distance
 * 10us pulse output in GPIO
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Ping_StartPing(void);



/** Ping_StartPing
 * @brief Read distance measurement
 * Send Pulse and capture distance
 * 10us pulse output in GPIO
 * @return 0 means not ready, else pulse width in cycles/distance
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
uint32_t Ping_Read(void); 


/** Cycles2milli
 * @brief Convert cycles to inches
 * Send Pulse and capture distance
 * 10us pulse output in GPIO
 * @return distance in .001 inches
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
uint32_t Cycles2milliInch(uint32_t cycles);



/** Cycles2millimeter
 * @brief Convert cycles to mm value
 * Send Pulse and capture distance (sound is 340.29 m/s)
 * 10us pulse output in GPIO
 * @return distance in mm
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
uint32_t Cycles2millimeter(uint32_t cycles); 



/** Ping_GetData
 * @brief Start distance measurement
 * Send Pulse and capture distance
 * 10us pulse output in GPIO
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Ping_GetData(uint32_t data[NUM_USONIC]);



  #endif 

