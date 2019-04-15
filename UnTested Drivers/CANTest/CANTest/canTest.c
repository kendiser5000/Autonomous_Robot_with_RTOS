

void CANReceive(void) {
	CANID receiveID;
	unsigned char canData[4];
	CAN0_Open();
	while (1) {
		CAN0_GetMail(&receiveID, canData);
		switch(receiveID) {
			case IRSensor:
				ST7735_Message(0,0,"IR0: ", ((unsigned short *)canData)[0]);
				//dataReceived++;
			break;
			case UltraSonic:
				ST7735_Message(0,1,"ULS0: ", ((unsigned long *)canData)[0]);
			break;
			case LaserSensor:
				ST7735_Message(0,1,"LS0: ", ((unsigned long *)canData)[0]);
			default:
			break;
}

void IRSensorSend(void) {	
	unsigned short IRvalues[4];
	unsigned long sonarValues[4];
	unsigned char CanData[4];

	IR_getValues(IRvalues);
	((unsigned short*)CanData)[0] = IRvalues[0];
	CAN0_SendData(IRSensor, CanData);

}