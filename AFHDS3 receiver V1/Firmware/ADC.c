#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D) 
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "Main.h"
#include "System definitions.h"
#include "System.h"

#include "ADC.h"

#if ADC_NB_CHANNELS!=1
	static unsigned char ADC_ChannelNb;
#endif
static unsigned char ADC_SampleNb;

static unsigned long ADC_ChannelsSumValue[ADC_NB_CHANNELS];
static unsigned long ADC_ChannelsValue[ADC_NB_CHANNELS];

static BOOL ADC_Initialized;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR12) || defined(PN_FBR8)
	static const unsigned char ADC_ChannelsNb[ADC_NB_CHANNELS]={6,7};
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D) 
	static const unsigned char ADC_ChannelsNb[ADC_NB_CHANNELS]={9,3};
#endif

void ADC_MsIRQHandler(void)
{
	if (!ADC_Initialized)
		return;
#if ADC_NB_CHANNELS==1
	// Sum the last conversion result
	if (ADC_SampleNb==0)
		ADC_ChannelsSumValue[0]=ADC1->DR;
	else
		ADC_ChannelsSumValue[0]+=ADC1->DR;
	ADC_SampleNb++;
	if (ADC_SampleNb>=ADC_NB_SAMPLES)
	{
		ADC_SampleNb=0;
		ADC_ChannelsValue[0]=ADC_ChannelsSumValue[0]/ADC_NB_SAMPLES;
	}
#else
	// Sum the last conversion result
	if (ADC_SampleNb==0)
		ADC_ChannelsSumValue[ADC_ChannelNb]=ADC1->DR;
	else
		ADC_ChannelsSumValue[ADC_ChannelNb]+=ADC1->DR;
	if (ADC_SampleNb==ADC_NB_SAMPLES-1)
		ADC_ChannelsValue[ADC_ChannelNb]=ADC_ChannelsSumValue[ADC_ChannelNb]/ADC_NB_SAMPLES;
	ADC_ChannelNb++;
	if (ADC_ChannelNb>=ADC_NB_CHANNELS)
	{
		ADC_ChannelNb=0;
		ADC_SampleNb++;
		if (ADC_SampleNb>=ADC_NB_SAMPLES)
			ADC_SampleNb=0;
	}
	ADC1->CHSELR=1<<ADC_ChannelsNb[ADC_ChannelNb];
#endif
	// Start the next conversion
	ADC1->CR|=ADC_CR_ADSTART;
	return;
}

// Return the ADC value converted to 1mV unit
unsigned long ADC_GetInternalVoltage(void)
{
	unsigned short InternalBatteryMonitorCorrection;
	
	InternalBatteryMonitorCorrection=(APP_Config.InternalBatteryMonitorCorrection!=0)?
		APP_Config.InternalBatteryMonitorCorrection*2:LNK_FactoryConfig.InternalBatteryMonitorCorrection;
	return (((ADC_ChannelsValue[0]*316877+(1<<15))>>16)*InternalBatteryMonitorCorrection+(1<<14))>>15;
}

// Return the ADC value converted to 1mV unit
unsigned long ADC_GetExternalVoltage(void)
{
#if ADC_NB_CHANNELS!=1
	unsigned short ExternalBatteryMonitorCorrection;
	
	ExternalBatteryMonitorCorrection=(APP_Config.ExternalBatteryMonitorCorrection!=0)?
		APP_Config.ExternalBatteryMonitorCorrection*2:LNK_FactoryConfig.ExternalBatteryMonitorCorrection;
	return ((ADC_ChannelsValue[1]*18+((ADC_ChannelsValue[1]*33887+(1<<15))>>16))*ExternalBatteryMonitorCorrection+(1<<14))>>15;
#else
	return 5000;
#endif
}

void ADC_Init(void)
{
#if ADC_NB_CHANNELS!=1
	ADC_ChannelNb=0;
#endif
	ADC_SampleNb=0;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8)
	// Set PA6 and PA7 to analog mode
	GPIOA->MODER=(GPIOA->MODER&~((3UL<<(6*2)) | (3UL<<(7*2)))) | (3<<(6*2)) | (3<<(7*2));
	RCC->APBENR2|=RCC_APBENR2_ADCEN;
	RCC->APBRSTR2|=RCC_APBRSTR2_ADCRST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_ADCRST;
#elif defined(PN_FBR12) || defined(PN_FBR4)
	// Set PA6 to analog mode
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(6*2))) | (3<<(6*2));
	RCC->APBENR2|=RCC_APBENR2_ADCEN;
	RCC->APBRSTR2|=RCC_APBRSTR2_ADCRST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_ADCRST;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D) 
	// Set PB1 and PA3 to analog mode
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(1*2))) | (3<<(1*2));
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(3*2))) | (3<<(3*2));
	RCC->APBENR2|=RCC_APBENR2_ADCEN;
	RCC->APBRSTR2|=RCC_APBRSTR2_ADCRST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_ADCRST;
#elif defined(PN_GMR)
	// Set PA6 to analog mode
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(6*2))) | (3<<(6*2));
	RCC->APB2ENR|=RCC_APB2ENR_ADCEN;
	RCC->APB2RSTR|=RCC_APB2RSTR_ADCRST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_ADCRST;
#endif
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
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D) 
	// OVSE=1 Oversampler enabled
	// OVSR=3 Oversampling ratio 16x
	// OVSS=4 Oversampling shift 4 bits
	// TOVS=0 All oversampled conversions for a channel are done consecutively after a trigger
	// LFTRIG=0 Low Frequency Trigger Mode disabled
	ADC1->CFGR2=ADC_CFGR2_OVSE | (ADC_CFGR2_OVSR_0*3) | (ADC_CFGR2_OVSS_0*4) | (ADC_CFGR2_CKMODE_0*1);
#elif defined(PN_GMR)
	// Set PCLK/4 as ADC clock source (12MHz)
	ADC1->CFGR2=ADC_CFGR2_CKMODE_1;
#endif
#if ADC_NB_CHANNELS==1
	// Set the unique ADC channel source
	ADC1->CHSELR=1<<ADC_BATTERY_MONITOR_CHANNEL_NB;
#else
	// Set the first ADC channel source
	ADC1->CHSELR=1<<ADC_ChannelsNb[0];
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D) 
	// SMP1=7 Sampling time selection 1 160.5 ADC clock cycles
	// SMP2=0 Not used
	// SMPSEL[18:0]=0 Sampling time of all channels use the setting of SMP1[2:0] register
	ADC1->SMPR=(ADC_SMPR_SMP1_0*3); 
#elif defined(PN_GMR)
	// Set the sample time to 28.5 cycles
	ADC1->SMPR|=(ADC_SMPR1_SMPR_0*3); 
#endif
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
