/**
* @file can0.h
* @brief Drivers for controlling CAN interface, ifndef for send/receive
	PE5 CAN Module to transmit
	PE4 Receive CAN Module
* @author Sikender
* @date 4/14/2019
*/

#ifndef __CAN0_H__
#define __CAN0_H__

/* Set Bitrate */
#define CAN_BITRATE  500000

/* @var CAN
 * To set device as receiver(motor board) [1] or transmitter (receiver board) [0] **/
#define RECEIVER 0

/** CANID
 * enum for CAN IDs
*/
typedef enum CANID_t {
	LaserSensor1 = 0x1,
	LaserSensor2 = 0x2,
	LaserSensor3 = 0x3,
	LaserSensor4 = 0x4,
	Motors		 = 0x5,
	TEST		 = 0x6
}CANID;//


/** CAN0_Open
 * @brief Open and initialize CAN ports
*/
void CAN0_Open(void);//
/* Enable Receiver Function */

#if RECEIVER
/** CAN0_Open
 * @brief receive data from CAN port
*/
void CAN0_GetMail(CANID *receiveID, unsigned char data[4]);//
//
/* Enable Transmitting functionality */
#else

/** CAN0_SendData
 * @brief Send data to other microcontroller
*/
void CAN0_SendData(CANID sendID, unsigned char data[4]);//
#endif
//
#endif
//

