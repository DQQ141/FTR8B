#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "LNK timer.h"

static unsigned long LNKTIM_TimerCounter;
static unsigned long LNKTIM_TimerCounterMarker;
static unsigned long LNKTIM_RemainingTime;

static tLNKTIM_TimerCompareIRQHandler *pLNKTIM_TimerCompareIRQHandler;

#if defined(BOOTLOADER)
void TIM17_IRQHandler2(void)
#else
void TIM17_IRQHandler(void)
#endif
{
	if (TIM17->SR&TIM_SR_CC1IF)
	{
		// Reset the TIM17 compare interrupt flag
		TIM17->SR=~TIM_SR_CC1IF;
		if (LNKTIM_RemainingTime==0)
		{
			if (pLNKTIM_TimerCompareIRQHandler)
				pLNKTIM_TimerCompareIRQHandler();
		}
		else
			LNKTIM_SetNextCompareIRQTimeInternal();
	}
	return;
}

unsigned short LNKTIM_GetCounter(void)
{
	return (unsigned short)TIM17->CNT;
}

void LNKTIM_Init(tLNKTIM_TimerCompareIRQHandler *pTimerCompareIRQHandler)
{
	pLNKTIM_TimerCompareIRQHandler=pTimerCompareIRQHandler;
	LNKTIM_TimerCounter=0;
	LNKTIM_RemainingTime=0;
	// Set TIM17 to be an up counter with a 1us increment time
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	RCC->APBENR2|=RCC_APBENR2_TIM17EN;
	RCC->APBRSTR2|=RCC_APBRSTR2_TIM17RST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_TIM17RST;
#elif defined(PN_GMR)
	RCC->APB2ENR|=RCC_APB2ENR_TIM17EN;
	RCC->APB2RSTR|=RCC_APB2RSTR_TIM17RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_TIM17RST;
#endif
	// CEN=0 counter disabled
	// UDIS=0 Update enabled
	// URS=0 Update request from any source
	// OPM=0 Counter is not stopped at update event
	// ARPE=0 Auto-reload preload disnabled
	// UIFREMAP=0 No remapping. UIF status bit is not copied to TIMx_CNT register bit 31
	TIM17->CR1=0;
	// Disable all channels output
	TIM17->CCER=0;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Not used
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Output frozen
	TIM17->CCMR1=0;
	// Set a 1MHz counter
	TIM17->PSC=SYSCLK/1000000-1;
	// Set maximum counter range
	TIM17->ARR=0xFFFF;
	// Generate an update event to update the prescaler and ARR
	TIM17->EGR=TIM_EGR_UG;
	// Enable the compare interrupt
	TIM17->DIER|=TIM_DIER_CC1IE;
	// Set the TIM17_IRQn IRQ to middle priority
	NVIC_SetPriority(TIM17_IRQn,IRQ_PRI_HIGH);
}

void LNKTIM_SetNextCompareIRQTime(unsigned long Time)
{
	LNKTIM_RemainingTime=Time;
	LNKTIM_SetNextCompareIRQTimeInternal();
	return;
}

void LNKTIM_SetNextCompareIRQTimeFromMarker(unsigned long Time)
{
	LNKTIM_TimerCounter=LNKTIM_TimerCounterMarker;
	LNKTIM_RemainingTime=Time;
	LNKTIM_SetNextCompareIRQTimeInternal();
	return;
}

void LNKTIM_SetNextCompareIRQTimeInternal(void)
{
	if (LNKTIM_RemainingTime<=0x10000)
	{
		LNKTIM_TimerCounter=(LNKTIM_TimerCounter+LNKTIM_RemainingTime)&0xFFFF;
		LNKTIM_RemainingTime=0;
	}
	else if (LNKTIM_RemainingTime<=0x20000)
	{
		unsigned long NextTime;
		
		NextTime=LNKTIM_RemainingTime/2;
		LNKTIM_TimerCounter=(LNKTIM_TimerCounter+NextTime)&0xFFFF;
		LNKTIM_RemainingTime-=NextTime;
	}
	else
		LNKTIM_RemainingTime-=0x10000;
	TIM17->CCR1=LNKTIM_TimerCounter;
	// Reset the TIM17 compare interrupt flag
	TIM17->SR=~TIM_SR_CC1IF;
	return;
}

void LNKTIM_SetTimeMarker(void)
{
	LNKTIM_TimerCounterMarker=TIM17->CNT;
	return;
}

void LNKTIM_StartTimer(unsigned long FirstCompareIRQTime)
{
	// Disable TIM17 counter
	TIM17->CR1&=~TIM_CR1_CEN;
	// Reset the TIM17 compare interrupt flag
	TIM17->SR=~TIM_SR_CC1IF;
	// Generate an update event to reset the TIM17 counter
	TIM17->EGR=TIM_EGR_UG;
	LNKTIM_TimerCounter=0;
	// Set the next compare IRQ time
	LNKTIM_RemainingTime=FirstCompareIRQTime;
	LNKTIM_SetNextCompareIRQTimeInternal();
	// Enable the TIM17_IRQn interrupt
	NVIC_EnableIRQ(TIM17_IRQn);
	// Enable TIM17 counter
	TIM17->CR1|=TIM_CR1_CEN;
	return;
}

void LNKTIM_StopTimer(void)
{
	// Disable TIM17 counter
	TIM17->CR1&=~TIM_CR1_CEN;
	return;
}
