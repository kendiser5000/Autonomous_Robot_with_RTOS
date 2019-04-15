/**
* @file can0.c
* @brief Drivers for controlling CAN interface, ifndef for send/receive
	PE5 CAN Module to transmit
	PE4 Receive CAN Module
* @author Sikender
* @date 4/14/2019
**/

#include "hw_can.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "can.h"
#include "debug.h"
#include "interrupt.h"

#include "OS.h"
#include "FIFO.h"
#include "can0.h"
#include "tm4c123gh6pm.h"

#define NULL 0

//******************************Receiver Stuff Only**********************************///
#if RECEIVER

#define RCVFIFOSUCCESS 1
#define RCVFIFOFAIL    0
#define RCVFIFOSIZE    32

/* DataRCVFrame
 * Structure to receive data from CAN
**/
typedef struct {
	CANID RCVID;	/* ID of data **/
	unsigned char RCVData[4];	/*data received **/
} DataRCVFrame;
/* create FIFO for receiving data received */
AddIndexFifo(RCV,RCVFIFOSIZE, DataRCVFrame, RCVFIFOSUCCESS, RCVFIFOFAIL)
sema4_t DataRdy;
#endif


/** CAN0_Handler
 * @brief Open and initialize CAN ports
*/
void CAN0_Handler(void){ unsigned char data[4];
  unsigned long ulIntStatus, ulIDStatus;
  int i;
  tCANMsgObject xTempMsgObject;
  xTempMsgObject.pucMsgData = data;
  ulIntStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
  if(ulIntStatus & CAN_INT_INTID_STATUS){
    ulIDStatus = CANStatusGet(CAN0_BASE, CAN_STS_NEWDAT);
    for(i = 0; i < 32; i++){
      if( (0x1 << i) & ulIDStatus){
				// actually care about data if this is the receive data
				#if RECEIVER
				DataRCVFrame newFrame;
				#endif
				CANMessageGet(CAN0_BASE, (i+1), &xTempMsgObject, true);
				#if RECEIVER
				newFrame.RCVID = (CANID) xTempMsgObject.ulMsgID;
				newFrame.RCVData[0] = data[0];
				newFrame.RCVData[1] = data[1];
				newFrame.RCVData[2] = data[2];
				newFrame.RCVData[3] = data[3];
				RCVFifo_Put(newFrame);
				OS_Signal(&DataRdy);
				#endif
      }
    }
  }
  CANIntClear(CAN0_BASE, ulIntStatus);  // acknowledge
}

/** CAN0_Setup_Message_Object
 * @brief Setup Can message object
*/
void static CAN0_Setup_Message_Object( unsigned long MessageID, \
                                unsigned long MessageFlags, \
                                unsigned long MessageLength, \
                                unsigned char * MessageData, \
                                unsigned long ObjectID, \
                                tMsgObjType eMsgType){
  tCANMsgObject xTempObject;
  xTempObject.ulMsgID = MessageID;          // 11 or 29 bit ID
  xTempObject.ulMsgLen = MessageLength;
  xTempObject.pucMsgData = MessageData;
  xTempObject.ulFlags = MessageFlags;
  CANMessageSet(CAN0_BASE, ObjectID, &xTempObject, eMsgType);
}


/** CAN0_Open
 * @brief Open and initialize CAN ports
*/
void CAN0_Open(void){unsigned long volatile delay;
  #if RECEIVER // Receiver
	RCVFifo_Init();
  #endif

  OS_InitSemaphore(&DataRdy, 0);
  SYSCTL_RCGCCAN_R |= 0x00000001;  // CAN0 enable bit 0
  SYSCTL_RCGCGPIO_R |= 0x00000010;  // RCGC2 portE bit 4
  for(delay=0; delay<100; delay++){};
  GPIO_PORTE_AFSEL_R |= 0x30; //PORTE AFSEL bits 5,4
// PORTE PCTL 88 into fields for pins 5,4
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF00FFFF)|0x00880000;
  GPIO_PORTE_DEN_R |= 0x30;
  GPIO_PORTE_DIR_R |= 0x20;
  CANInit(CAN0_BASE);
  CANBitRateSet(CAN0_BASE, 80000000, CAN_BITRATE);
  CANEnable(CAN0_BASE);
// make sure to enable STATUS interrupts
  CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
// Set up filter to receive these IDs
// in this case there is just one type, but you could accept multiple ID types
  //CAN0_Setup_Message_Object(RCV_ID, MSG_OBJ_RX_INT_ENABLE, 4, NULL, RCV_ID, MSG_OBJ_TYPE_RX);
	#if RECEIVER
  CAN0_Setup_Message_Object((unsigned long) (IRSensor)		, MSG_OBJ_RX_INT_ENABLE, 4, NULL, (unsigned long) (IRSensor)		, MSG_OBJ_TYPE_RX);
  CAN0_Setup_Message_Object((unsigned long) (PingSensor)	, MSG_OBJ_RX_INT_ENABLE, 4, NULL, (unsigned long) (PingSensor)	, MSG_OBJ_TYPE_RX);
	CAN0_Setup_Message_Object((unsigned long) (LaserSensor)	, MSG_OBJ_RX_INT_ENABLE, 4, NULL, (unsigned long) (LaserSensor)	, MSG_OBJ_TYPE_RX);
	#endif

  NVIC_EN1_R = (1 << (INT_CAN0 - 48)); //IntEnable(INT_CAN0);
  return;
}



#if RECEIVER
/** CAN0_GetMail
 * @brief Open and initialize CAN ports
*/
void CAN0_GetMail(CANID *receiveID, unsigned char data[4]){
	DataRCVFrame data;
	OS_Wait(&DataRdy);
	RCVFifo_Get(&data);
	*receiveID = data.RCVID;
	data[0] = data.RCVData[0];
	data[1] = data.RCVData[1];
	data[2] = data.RCVData[2];
	data[3] = data.RCVData[3];
}

#else
/** CAN0_SendData
 * @brief Open and initialize CAN ports
*/
void CAN0_SendData(CANID sendID, unsigned char data[4]){
// in this case there is just one type, but you could accept multiple ID types
  CAN0_Setup_Message_Object((unsigned long) sendID, NULL, 4, data, (unsigned long) sendID, MSG_OBJ_TYPE_TX);
}

#endif
