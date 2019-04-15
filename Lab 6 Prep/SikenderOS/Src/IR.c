// IR.c
// Created On:  04/10/19
// Modified On: 04/10/19
// Created By:  Richard Li
#include "ADC.h"
#include "IR.h"
#include <stdio.h>
#include <stdlib.h>

#define FILTERSIZE	3
#define skewRate	0
#define fs 			100
uint16_t medianFilter[FILTERSIZE];
int middle;
int buffIndex;

void IR_Task(unsigned long data);

void IR_Init(uint32_t channelNum){
	//ADC0_InitTimer2ATriggerSeq3(channelNum, 80000000 / fs);
	ADC_Collect(channelNum, fs, IR_Task);
	middle = FILTERSIZE / 2;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//get IR filtered value
int IR_getValue(void){
	//qsort(medianFilter, FILTERSIZE, sizeof(int), cmpfunc);
	return medianFilter[middle];
}

void IR_Task(unsigned long data){
	medianFilter[buffIndex] = data;
  buffIndex = (buffIndex + 1) % FILTERSIZE;
}
