/** @file Ping.c
 * @brief Drivers for Ping Sensor
 * @date 4/9/2018
 * @author Sikender Ashraf
 */

 #ifndef __Ping_H__
 #define __Ping_H__


 /** Ping_PORTB_Initilization
  * @brief Initialize Ping sensor ports
  * @parameter task task to run when sensor sends signal
  * @date 4/9/2018
  * @author Sikender Ashraf
  */
void Ping_Initilizations(void(*task)(void));


/** Ping_PORTB_Start
 * @brief Start Signal for Ping Sensor
 * @parameter task task to run when sensor sends signal
 * @date 4/9/2018
 * @author Sikender Ashraf
 */
void Ping_PORTB_Start(void);








  #endif __Ping_H__
