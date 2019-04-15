// ADC.h
// Created On:  04/10/19
// Modified On: 04/10/19
// Created By:  Richard Li


//----------ADC_Open----------
// Input:   channelNum
// Output:  0 for good, -1 for channelNum parse error
void ADC_Open(void);

//----------ADC_In----------
// Input:   none
// Output:  value of sample
uint16_t ADC_In(void);

//----------ADC_Collect----------
// Input:   channelNum, fs, buffer[], numberOfSamples
// Output:  none
void ADC_Collect(uint32_t channelNum, uint32_t fs, uint16_t buffer[], uint32_t numberOfSamples);
