
char* USONICStrings[3] = {"US0: ", "US1: ", "US2: "};
uint32_t USONICValues[NUM_USONIC];
void USONICSend(void){
	uint8_t message[8];
	Ping_Init();
	OS_AddPeriodicThread(&Ping_StartHCSR04, BUSCLK/20, 0); //20 Hz data Collection
	while(1){
		//PF1 ^= 0x02;
		Ping_GetData(USONICValues);
		for(int i=0; i<NUM_USONIC; i++){
			message[2*i] = (USONICValues[i]&0x00FF);
			message[2*i+1] = ((USONICValues[i]&0xFF00)>>8);
			ST7735_Message(1, i, USONICStrings[i], USONICValues[i]);
		}
	  //CAN0_SendData(message);
	}
}