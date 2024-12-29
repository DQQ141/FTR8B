#include <stm32g0xx.h>

#include "System.h"
#include "ADC.h"


static BOOL ADC_Initialized;

void ADC_Init(void)
{
	// Set PA7 to analog mode
	GPIOA->MODER=(GPIOA->MODER&~((3UL<<(7*2)))) | (3<<(7*2));
	RCC->APBENR2|=RCC_APBENR2_ADCEN;
	RCC->APBRSTR2|=RCC_APBRSTR2_ADCRST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_ADCRST;
	// Disable the ADC if previously enabled
	if (ADC1->CR&ADC_CR_ADEN)
	{
		// Ongoing conversion?
		if (ADC1->CR&ADC_CR_ADSTART)
		{
			ADC1->CR|=ADC_CR_ADSTP;
			while (ADC1->CR&ADC_CR_ADSTP);
		}
		// Disable the ADC
		ADC1->CR|=ADC_CR_ADDIS;
		while (ADC1->CR&ADC_CR_ADEN);
	}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D) 
	// Enable the ADC internal voltage regulator
	ADC1->CR|=ADC_CR_ADVREGEN;
	// Wait for the regurator startup time
	SYS_DelayUs(1000);
#endif
	// Calibrate the ADC
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR&ADC_CR_ADCAL);
	ADC1->CR|=ADC_CR_ADEN;
	// Enable the ADC
	ADC1->CR|=ADC_CR_ADEN;
	while (!(ADC1->ISR&ADC_ISR_ADRDY));
// OVSE=1 Oversampler enabled
	// OVSR=3 Oversampling ratio 16x
	// OVSS=4 Oversampling shift 4 bits
	// TOVS=0 All oversampled conversions for a channel are done consecutively after a trigger
	// LFTRIG=0 Low Frequency Trigger Mode disabled
	ADC1->CFGR2=ADC_CFGR2_OVSE | (ADC_CFGR2_OVSR_0*3) | (ADC_CFGR2_OVSS_0*4) | (ADC_CFGR2_CKMODE_0*1);

	// Set the first ADC channel source
	ADC1->CHSELR=1<<7;

// SMP1=7 Sampling time selection 1 160.5 ADC clock cycles
	// SMP2=0 Not used
	// SMPSEL[18:0]=0 Sampling time of all channels use the setting of SMP1[2:0] register
	ADC1->SMPR=(ADC_SMPR_SMP1_0*3); 

	// DMAEN=0 Direct memory access disabled
	// DMACFG=0 Not used
	// SCANDIR=0 Scan sequence upward
	// RES=0 Data resolution 12 bits
	// ALIGN=0 Data aligned to the right
	// EXTSEL=0 Not used
	// EXTEN=0 Hardware trigger detection disabled
	// OVRMOD=0 ADC_DR register is preserved with the old data when an overrun is detected
	// CONT=0 Single conversion mode
	// WAIT=0 Wait conversion mode off
	// AUTOFF=0 Auto-off mode disabled
	// DISCEN=0 Discontinuous mode disabled
	// CHSELRMOD=0 Each bit of the ADC_CHSELR register enables an input
	// AWD1SGL=0 Analog watchdog 1 enabled on all channels
	// AWD1EN=0 Analog watchdog 1 disabled
	// AWD1CH=0 Not used
	ADC1->CFGR1=0;
	ADC_Initialized=TRUE;
	return;
}
