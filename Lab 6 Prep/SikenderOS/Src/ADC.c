// ADC.c
// Created On:  04/10/19
// Modified On: 04/10/19
// Created By:  Richard Li

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"
#define BUSCLK          80000000


int StartCritical(void);
void EndCritical(int sr);

int         SampleNums;
uint16_t*    ADCBuff;
int         buffIndex;

//----------ADC_Open----------
void ADC_Open(void) {
    volatile uint32_t delay;
    // pin initialization
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;        // 1) activate clock
    delay = SYSCTL_RCGCGPIO_R;  // clock stabilize
    delay = SYSCTL_RCGCGPIO_R;  // clock stabilize
            
    GPIO_PORTE_DIR_R    &= ~0x01;
    GPIO_PORTE_AFSEL_R  |= 0x01;
    GPIO_PORTE_DEN_R    &= ~0x01;
    GPIO_PORTE_AMSEL_R  |= 0x01;
        
    GPIO_PORTE_PCTL_R = ~0x0000FFFF;
}

//----------ADC_In----------
// Input:   none
// Output:  value of sample
uint16_t ADC_In(void) {
    uint32_t data;
    ADC0_PSSI_R =0x0004;
    while((ADC0_RIS_R & 0x04) == 0) {};
    data = ADC0_SSFIFO2_R & 0xFFF;
    ADC0_ISC_R = 0x0004;
    return data;
}

//----------ADC_Collect----------
// Input:   channelNum, fs, buffer[], numberOfSamples
// Output:  none
void ADC_Collect(uint32_t channelNum, uint32_t fs, uint16_t *buffer, uint32_t numberOfSamples){

        volatile uint32_t delay;

        ADCBuff = buffer;
        SampleNums = numberOfSamples;
        buffIndex = 0;
    
        long status = StartCritical();
        SYSCTL_RCGCADC_R |= 0x01;     // activate ADC0 
        SYSCTL_RCGCTIMER_R |= 0x04;   // activate timer0 
        delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
        TIMER2_CTL_R = 0x00000000;    // disable timer0A during setup
        TIMER2_CTL_R |= 0x00000020;   // enable timer0A trigger to ADC
        TIMER2_CFG_R = 0;             // configure for 32-bit timer mode
        TIMER2_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
        TIMER2_TAPR_R = 0;            // prescale value for trigger
        TIMER2_TAILR_R = (BUSCLK/fs)-1;    // start value for trigger
        TIMER2_IMR_R = 0x00000000;    // disable all interrupts
        TIMER2_CTL_R |= 0x00000001;   // enable timer0A 32-b, periodic, no interrupts
        ADC0_PC_R = 0x01;         // configure for 125K samples/sec
        ADC0_SSPRI_R = 0x3210;    // sequencer 0 is highest, sequencer 3 is lowest
        ADC0_ACTSS_R &= ~0x08;    // disable sample sequencer 3
        ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFF0FFF)+0x5000; // timer trigger event
        ADC0_SSMUX3_R = channelNum;
        ADC0_SSCTL3_R = 0x06;          // set flag and end                       
        ADC0_IM_R |= 0x08;             // enable SS3 interrupts
        ADC0_ACTSS_R |= 0x08;          // enable sample sequencer 3
        NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFF00FF)|0x00004000; //priority 2
        NVIC_EN0_R = 1<<17;              // enable interrupt 17 in NVIC
        EndCritical(status);
}

//-----------ADC0 Sequencer----------
void ADC0Seq3_Handler(void) {
    ADC0_ISC_R = 0x08;  //acknowledge

    ADCBuff[buffIndex] = ADC_In();
    buffIndex = (buffIndex + 1) % SampleNums;
}
