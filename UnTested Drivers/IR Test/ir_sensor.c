// IR_sensor.c

#include "ir_sensor.h"

#include "PLL.h"
#include "SysTick.h"
#include "OS.h"
#include "FIFO_sema4.h"
#include "semaphore.h"
#include "myadc.h"
#include "ST7735.h"
#include "interpreter.h"

#include "gpio_debug.h"

#define SAMPLING_RATE         2000 // in unit of Hz
#define TIMESLICE 2*TIME_1MS  // thread switch time in system time units

static unsigned short data[4];

#define APPLY_FILTER 0

#if APPLY_FILTER
/************************* Median Filter *******************/
static unsigned short median5(unsigned short *buf5){
	unsigned short *bufm=buf5;
	int temp=0, i=0, j=0;

	for(j=0;j<5;j++){
		for(i=0;i<5;i++){
			if(bufm[i]>bufm[i+1]){
				temp=bufm[i];
				bufm[i]=bufm[i+1];
				bufm[i+1]=temp;
			}
		}
	}

	return	bufm[2];
}

typedef struct {
	unsigned short buf[10];
	unsigned char index;
} MedFilter;

MedFilter filter0 = {{0}, 9};
MedFilter filter1 = {{0}, 9};
MedFilter filter2 = {{0}, 9};
MedFilter filter3 = {{0}, 9};

unsigned short MedianFilter(MedFilter *f, unsigned short n) {
	unsigned char i = f->index;
	if(++f->index == 10) f->index = 5;
	
	f->buf[i-5] = f->buf[i] = n;
	return median5(&f->buf[i-5+1]);
}

#endif

/********************* Calibration detail ******************/
#define Calibtable_len       13
static const short Calitable[Calibtable_len+1] = {
	2930,
	2020,
	1600,
	1300,
	1110,
	980,
	860,
	780,
	720,
	660,
	620,
	560,
	540,
	
	-1, // min
};

// Calidiff[i] = Calitable[i] - Calitable[i+1]
static const short Calidiff[Calibtable_len-1] = {
	910,
	420,
	300,
	190,
	130,
	120,
	80,
	60,
	60,
	40,
	60,
	20,

};

static unsigned char calibrate(unsigned short adcval) {
	int i;
	
	for(i = 0; adcval <= Calitable[i] ; i++);
	// now, C[i] < adcval <= C[i+1]
	
	// saturation
	if (i == 0) return 10;
	else if(i == Calibtable_len) return 70;
	else return (unsigned char) (10+5*(i-1) + 5*(adcval - Calitable[i])/(Calidiff[i-1]));
}



// Call back function passed to ADC
void  IRCallBack(unsigned short buf[]) {
	
	data[0] = buf[0];
	data[1] = buf[1];
	data[2] = buf[2];
	data[3] = buf[3];
}


void IR_Init(void) {
	
  myADC_Collect4(SAMPLING_RATE, IRCallBack);
}

void IR_getValues (unsigned char *buffer) {
	#if APPLY_FILTER
	buffer[0] = calibrate(MedianFilter(&filter0, data[0]));
	buffer[1] = calibrate(MedianFilter(&filter1, data[1]));
	buffer[2] = calibrate(MedianFilter(&filter2, data[2]));
	buffer[3] = calibrate(MedianFilter(&filter3, data[3]));
	#else
	buffer[0] = calibrate(data[0]);
	buffer[1] = calibrate(data[1]);
	buffer[2] = calibrate(data[2]);
	buffer[3] = calibrate(data[3]);
	#endif
}
