/**
* @file ADC.h
* @brief Wrapper for ADC drivers
*
* @author Sikender & Sijin
*
* @date 2/04/2019
*/

#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "OS.h"



#define NVIC_EN0_INT17          0x00020000  // Interrupt 17 enable
#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAOTE         0x00000020  // GPTM TimerA Output Trigger
                                            // Enable
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt
                                            // Mask
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low
#define ADC_ACTSS_ASEN3         0x00000008  // ADC SS3 Enable
#define ADC_RIS_INR3            0x00000008  // SS3 Raw Interrupt Status
#define ADC_IM_MASK3            0x00000008  // SS3 Interrupt Mask
#define ADC_ISC_IN3             0x00000008  // SS3 Interrupt Status and Clear
#define ADC_EMUX_EM3_M          0x0000F000  // SS3 Trigger Select mask
#define ADC_EMUX_EM3_TIMER      0x00005000  // Timer
#define ADC_SSPRI_SS3_4TH       0x00003000  // fourth priority
#define ADC_SSPRI_SS2_3RD       0x00000200  // third priority
#define ADC_SSPRI_SS1_2ND       0x00000010  // second priority
#define ADC_SSPRI_SS0_1ST       0x00000000  // first priority
#define ADC_PSSI_SS3            0x00000008  // SS3 Initiate
#define ADC_SSCTL3_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL3_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL3_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL3_D0           0x00000001  // 1st Sample Diff Input Select
#define ADC_SSFIFO3_DATA_M      0x00000FFF  // Conversion Result Data mask
#define ADC_PC_SR_M             0x0000000F  // ADC Sample Rate
#define ADC_PC_SR_125K          0x00000001  // 125 ksps
#define SYSCTL_RCGCGPIO_R4      0x00000010  // GPIO Port E Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R3      0x00000008  // GPIO Port D Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R1      0x00000002  // GPIO Port B Run Mode Clock
                                           // Gating Control
										   
										   
/** ADC0_InitTimer2ATriggerSeq3
 * Initializes ADC channel and collects data periodically using Timer2a
 * @param channelNum specifies channel number to open
 * @param period samling rate of ADC
 * @date 3/2/2019
 * @author Sikender & Sijin
 */
void ADC0_InitTimer2ATriggerSeq3(uint8_t channelNum, uint32_t period);




/** ADC0_InitSWTriggerSeq3
 * Setup ADC for SW called ADC
 * @param channelNum specifies channel number to open
 * @date 3/2/2019
 * @author Sikender & Sijin
 */
void ADC_InitSWTriggerSeq3(uint32_t channelNum);


/** ADC_In
 * Get data from ADC
 * @return ADC data
 * @date 3/2/2019
 * @author Sikender & Sijin
 */
uint16_t ADC_In(void);


/** ADC0_Status
 * Check status for ADC
 * @date 3/2/2019
 * @author Sikender & Sijin
 */
int ADC_Status(void);


/** ADC0_Collect
 * Collect ADC samples with HW interrupt for ADC
 * @param channelNum specifies channel number to open
 * @param fs sampling rate
 * @param task function to run when data collected
 * @date 3/2/2019
 * @author Sikender & Sijin
 */
void ADC_Collect(uint32_t channelNum, uint32_t fs, void(*task)(unsigned long));



/** ADC_Open
 * The following function opens an ADC Channel
 * @channelNum specifies channel to initialize
 * @returns 1 if success
 */
int ADC_Open(uint32_t channelNum);




#endif //  __ADC_H__

