// IR.h
// Created On:  04/10/19
// Modified On: 04/10/19
// Created By:  Richard Li

#include <stdint.h>

//----------IR_Init----------
//Initialize globals and ADC
void IR_Init(uint32_t channelNum);

//----------getIRValue--------
//return filtered IR value
int IR_getValue(void);
