#include <string.h>
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "LED.h"

#include "LED private.h"

static BOOL LED_Initialized;

static sLED_Pattern LED_Pattern;

static unsigned long LED_MsCounter;
static unsigned long LED_PatternBitNb;

const sLED_Pattern LED_Bind           	={0x00000002,2,100};
const sLED_Pattern LED_WaitBind       	={0x00000003,3,100};
const sLED_Pattern LED_Synchronizing  	={0x00000002,2,1000};
const sLED_Pattern LED_Synchronized   	={0x00000001,1,100};

const sLED_Pattern LED_Test           	={0x000000AA,12,100};
const sLED_Pattern LED_Factory        	={0x0000002A,10,100};
const sLED_Pattern LEDP_InternalError	={0x000002AA,15,100};
const sLED_Pattern LED_PWMA_NoSignal  	={0x000BFC00,20,100};
const sLED_Pattern LED_PWMA_Configured	={0x000BFFFF,20,100};

const sLED_Pattern LED_SVC_Calibration  ={0x0000000A,8,100};
const sLED_Pattern LED_FC_Calibration  ={0x0000000A,8,100};

void LED_MsIRQHandler(void)
{
	if (!LED_Initialized)
		return;
	LED_MsCounter++;
	if (LED_MsCounter<LED_Pattern.MsPerPatternBit)
		return;
	LED_MsCounter=0;
	LED_PatternBitNb++;
	if (LED_PatternBitNb>=LED_Pattern.NbPatternBits)
		LED_PatternBitNb=0;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	if (LED_Pattern.Pattern&(1<<LED_PatternBitNb))
		GPIOA->BSRR=1<<(16+12);
	else
		GPIOA->BSRR=1<<( 0+12);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	if (LED_Pattern.Pattern&(1<<LED_PatternBitNb))
		GPIOC->BSRR=1<<(16+6);
	else
		GPIOC->BSRR=1<<( 0+6);
#elif defined(PN_GMR)
	if (LED_Pattern.Pattern&(1<<LED_PatternBitNb))
		GPIOA->BSRR=1<<(16+5);
	else
		GPIOA->BSRR=1<<( 0+5);
#endif
	return;
}

void LED_Init(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set PA12 as output
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(12*2))) | (1<<(12*2));
	GPIOA->BSRR=1<<( 0+12);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// Set PC6 as output
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(6*2))) | (1<<(6*2));
	GPIOC->BSRR=1<<( 0+6);
#elif defined(PN_GMR)
	// Set PA5 as output
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(5*2))) | (1<<(5*2));
	GPIOA->BSRR=1<<( 0+5);
#endif
	LED_Initialized=TRUE;
	LED_SetPattern(&LEDP_InternalError);
	return;
}

void LED_SetPattern(const sLED_Pattern *pPattern)
{
	if (!memcmp(&LED_Pattern,pPattern,sizeof(sLED_Pattern)))
		return;
	LED_Pattern=*pPattern;
	LED_MsCounter=0;
	LED_PatternBitNb=pPattern->NbPatternBits-1;
	LED_MsCounter=pPattern->MsPerPatternBit-1;
	return;
}
