#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif
#include <string.h>

#include "System definitions.h"
#include "System.h"

#include "NPT timer.h"

#include "NPT timer private.h"

static BOOL NPTTIM_Initialized;

static unsigned long NPTTIM_TimerCounters[NPT_NB_CHANNELS];
static unsigned long NPTTIM_RemainingTimes[NPT_NB_CHANNELS];
static BOOL NPTTIM_NextCompareIRQsOutputLevel[NPT_NB_CHANNELS];
static tNPTTIM_TimerCompareIRQHandler *pNPTTIM_TimerCompareIRQHandlers[NPT_NB_CHANNELS];

// [Timer number]: one bit per timer channel, 0=low priority, 1=high priority
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8) 
	static unsigned char NPTTIM_AreIRQPrioritiesHigh[4];
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	static unsigned char NPTTIM_AreIRQPrioritiesHigh[5];
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_GMR) || defined(PN_FBR4)
	static unsigned char NPTTIM_AreIRQPrioritiesHigh[2];
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	static unsigned char NPTTIM_AreIRQPrioritiesHigh[3];
#elif defined(PN_TR8B)
	static unsigned char NPTTIM_AreIRQPrioritiesHigh[4];
#elif defined(PN_TMR)
	static unsigned char NPTTIM_AreIRQPrioritiesHigh[1];
#endif
// PWM Channel number to timer number and timer channel number conversion for IRQ priority management
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8) 
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={0,3,0,1,0,1,2,2};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={3,0,1,0,2,2,0,1};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM1_CC_IRQn,
		TIM16_IRQn,
		TIM1_CC_IRQn,
		TIM3_IRQn,
		TIM1_CC_IRQn,
		TIM3_IRQn,
		TIM15_IRQn,
		TIM15_IRQn,
	};
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={0,4,0,1,1,0,2,1,0,1,3,3};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={3,0,1,0,1,0,0,3,2,2,0,1};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM1_CC_IRQn,
		TIM16_IRQn,
		TIM1_CC_IRQn,
		TIM3_IRQn,
		TIM3_IRQn,
		TIM1_CC_IRQn,
		TIM14_IRQn,
		TIM3_IRQn,
		TIM1_CC_IRQn,
		TIM3_IRQn,
		TIM15_IRQn,
		TIM15_IRQn,
	};
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={0,0,1,1};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={2,1,0,1};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM1_CC_IRQn,
		TIM1_CC_IRQn,
		TIM15_IRQn,
		TIM15_IRQn
	};
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={2,0,1,0,1,0};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={1,3,1,1,2,2};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM3_IRQn,
		TIM1_CC_IRQn,
		TIM2_IRQn,
		TIM1_CC_IRQn,
		TIM2_IRQn,
		TIM1_CC_IRQn
	};
#elif  defined(PN_FGR4D)
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={1,0,1,0};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={1,1,2,2};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM2_IRQn,
		TIM1_CC_IRQn,
		TIM2_IRQn,
		TIM1_CC_IRQn
	};
#elif defined(PN_TR8B)
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={2,2,0,0,3,2,1,0};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={1,0,1,3,0,2,1,2};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM3_IRQn,
		TIM3_IRQn,
		TIM1_CC_IRQn,
		TIM1_CC_IRQn,
		TIM15_IRQn,
		TIM3_IRQn,
		TIM2_IRQn,
		TIM1_CC_IRQn
	};
#elif defined(PN_GMR)
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={0,0,1,0};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={0,3,2,1};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM1_CC_IRQn,
		TIM1_CC_IRQn,
		TIM2_IRQn,
		TIM1_CC_IRQn
	};
#elif defined(PN_TMR)
	static const unsigned char NPTTIM_PWMChannelToTimerNumber[NPT_NB_CHANNELS]={0};
	static const unsigned char NPTTIM_PWMChannelToTimerChannelNumber[NPT_NB_CHANNELS]={1};
	static const IRQn_Type NPTTIM_PWMChannelToTimerIRQNumber[NPT_NB_CHANNELS]={
		TIM1_CC_IRQn
	};
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
void TIM1_CC_IRQHandler(void)
{
	unsigned char AreIRQPrioritiesHigh;
	unsigned char TriggeredChannels;

	TriggeredChannels=((TIM1->SR & TIM1->DIER)>>TIM_SR_CC1IF_Pos)&0x0F;
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8) 
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 4]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 4])
				pNPTTIM_TimerCompareIRQHandlers[ 4]( 4,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 4);
	}
	if ((TriggeredChannels&(1<<3)) &&
		((AreIRQPrioritiesHigh & (1<<3)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC4IF;
		TriggeredChannels&=~(1<<3);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 5]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 5])
				pNPTTIM_TimerCompareIRQHandlers[ 5]( 5,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 5);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 8]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 8])
				pNPTTIM_TimerCompareIRQHandlers[ 8]( 8,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 8);
	}
	if ((TriggeredChannels&(1<<3)) &&
		((AreIRQPrioritiesHigh & (1<<3)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC4IF;
		TriggeredChannels&=~(1<<3);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB)|| defined(PN_FBR4)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 5]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 5])
				pNPTTIM_TimerCompareIRQHandlers[ 5]( 5,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 5);
	}
	if ((TriggeredChannels&(1<<3)) &&
		((AreIRQPrioritiesHigh & (1<<3)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC4IF;
		TriggeredChannels&=~(1<<3);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
#elif defined(PN_FGR4D)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}
#elif defined(PN_TR8B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 7]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 7])
				pNPTTIM_TimerCompareIRQHandlers[ 7]( 7,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 7);
	}
	if ((TriggeredChannels&(1<<3)) &&
		((AreIRQPrioritiesHigh & (1<<3)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC4IF;
		TriggeredChannels&=~(1<<3);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}

#elif defined(PN_GMR)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}
	if ((TriggeredChannels&(1<<3)) &&
		((AreIRQPrioritiesHigh & (1<<3)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC4IF;
		TriggeredChannels&=~(1<<3);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
#elif defined(PN_TMR)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[0];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM1->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
#endif
	return;
}
#endif

#if defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
void TIM2_IRQHandler(void)
{
	unsigned char AreIRQPrioritiesHigh;
	unsigned char TriggeredChannels;

	TriggeredChannels=((TIM2->SR & TIM2->DIER)>>TIM_SR_CC1IF_Pos)&0x0F;
#if defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM2->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM2->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 4]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 4])
				pNPTTIM_TimerCompareIRQHandlers[ 4]( 4,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 4);
	}
#elif defined(PN_FGR4D)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM2->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM2->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
#elif defined(PN_TR8B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM2->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 6]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 6])
				pNPTTIM_TimerCompareIRQHandlers[ 6]( 6,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 6);
	}
#elif defined(PN_GMR)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM2->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
#endif
	return;
}
#endif

#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B)\
	|| defined(PN_FBR8) 
void TIM3_IRQHandler(void)
{
	unsigned char AreIRQPrioritiesHigh;
	unsigned char TriggeredChannels;

	TriggeredChannels=((TIM3->SR & TIM3->DIER)>>TIM_SR_CC1IF_Pos)&0x0F;
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 5]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 5])
				pNPTTIM_TimerCompareIRQHandlers[ 5]( 5,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 5);
	}
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 4]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 4])
				pNPTTIM_TimerCompareIRQHandlers[ 4]( 4,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 4);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))

	{
		TIM3->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 9]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 9])
				pNPTTIM_TimerCompareIRQHandlers[ 9]( 9,AreIRQPrioritiesHigh);
		}
		else
		NPTTIM_SetNextCompareIRQTimeInternal( 9);
	}
	if ((TriggeredChannels&(1<<3)) &&
		((AreIRQPrioritiesHigh & (1<<3)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))

	{
		TIM3->SR=~TIM_SR_CC4IF;
		TriggeredChannels&=~(1<<3);
		if (NPTTIM_RemainingTimes[ 7]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 7])
				pNPTTIM_TimerCompareIRQHandlers[ 7]( 7,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 7);
	}
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[2];
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
#elif defined(PN_TR8B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[2];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM3->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 0]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 0])
				pNPTTIM_TimerCompareIRQHandlers[ 0]( 0,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 0);
	}
	if ((TriggeredChannels&(1<<2)) &&
		((AreIRQPrioritiesHigh & (1<<2)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))

	{
		TIM3->SR=~TIM_SR_CC3IF;
		TriggeredChannels&=~(1<<2);
		if (NPTTIM_RemainingTimes[ 5]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 5])
				pNPTTIM_TimerCompareIRQHandlers[ 5]( 5,AreIRQPrioritiesHigh);
		}
		else
		NPTTIM_SetNextCompareIRQTimeInternal( 5);
	}
#endif
	return;
}
#endif

#if defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
void TIM14_IRQHandler(void)
{
	unsigned char AreIRQPrioritiesHigh;
	unsigned char TriggeredChannels;

	TriggeredChannels=((TIM14->SR & TIM14->DIER)>>TIM_SR_CC1IF_Pos)&0x01;
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[2];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM14->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 6]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 6])
				pNPTTIM_TimerCompareIRQHandlers[ 6]( 6,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 6);
	}
	return;
}
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4)
void TIM15_IRQHandler(void)
{
	unsigned char AreIRQPrioritiesHigh;
	unsigned char TriggeredChannels;

	TriggeredChannels=((TIM15->SR & TIM15->DIER)>>TIM_SR_CC1IF_Pos)&0x03;
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[2];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 6]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 6])
				pNPTTIM_TimerCompareIRQHandlers[ 6]( 6,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 6);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 7]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 7])
				pNPTTIM_TimerCompareIRQHandlers[ 7]( 7,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 7);
	}
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[3];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[10]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[10])
				pNPTTIM_TimerCompareIRQHandlers[10](10,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal(10);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[11]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[11])
				pNPTTIM_TimerCompareIRQHandlers[11](11,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal(11);
	}
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB)|| defined(PN_FBR4)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[1];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 2]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 2])
				pNPTTIM_TimerCompareIRQHandlers[ 2]( 2,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 2);
	}
	if ((TriggeredChannels&(1<<1)) &&
		((AreIRQPrioritiesHigh & (1<<1)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC2IF;
		TriggeredChannels&=~(1<<1);
		if (NPTTIM_RemainingTimes[ 3]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 3])
				pNPTTIM_TimerCompareIRQHandlers[ 3]( 3,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 3);
	}
#elif defined(PN_TR8B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[3];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM15->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 4]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 4])
				pNPTTIM_TimerCompareIRQHandlers[ 4]( 4,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 4);
	}
#endif
	return;
}
#endif

#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)|| defined(PN_FBR8) 
void TIM16_IRQHandler(void)
{
	unsigned char AreIRQPrioritiesHigh;
	unsigned char TriggeredChannels;

	TriggeredChannels=((TIM16->SR & TIM16->DIER)>>TIM_SR_CC1IF_Pos)&0x01;
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[3];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM16->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	AreIRQPrioritiesHigh=NPTTIM_AreIRQPrioritiesHigh[4];
	if ((TriggeredChannels&(1<<0)) &&
		((AreIRQPrioritiesHigh & (1<<0)) ||
		 !(TriggeredChannels & AreIRQPrioritiesHigh)))
	{
		TIM16->SR=~TIM_SR_CC1IF;
		TriggeredChannels&=~(1<<0);
		if (NPTTIM_RemainingTimes[ 1]==0)
		{
			if (pNPTTIM_TimerCompareIRQHandlers[ 1])
				pNPTTIM_TimerCompareIRQHandlers[ 1]( 1,AreIRQPrioritiesHigh);
		}
		else
			NPTTIM_SetNextCompareIRQTimeInternal( 1);
	}
#endif
	return;
}
#endif

// Set the channel to frozen mode
void NPTTIM_DisableChannel(unsigned long ChannelNb,BOOL NoOutput)
{
	SYS_DisableIRQs();
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8) 
	case 0:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		TIM1->DIER&=~TIM_DIER_CC4IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (0<<(11*2));
		break;
	case 1:
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM16->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOD->MODER=(GPIOD->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 2:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 3*2))) | (0<<( 3*2));
		break;
	case 3:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM3->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 4*2))) | (0<<( 4*2));
		break;
	case 4:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM1->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 6*2))) | (0<<( 6*2));
		break;
	case 5:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM3->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 6:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM15->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 7:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM15->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (0<<(15*2));
		break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		TIM1->DIER&=~TIM_DIER_CC4IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (0<<(11*2));
		break;
	case 1:
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM16->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOD->MODER=(GPIOD->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 2:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 3*2))) | (0<<( 3*2));
		break;
	case 3:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM3->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 4*2))) | (0<<( 4*2));
		break;
	case 4:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM3->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 5*2))) | (0<<( 5*2));
		break;
	case 5:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM1->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 8*2))) | (0<<( 8*2));
		break;
	case 6:
		TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM14->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 4*2))) | (0<<( 4*2));
		break;
	case 7:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk);
		TIM3->DIER&=~TIM_DIER_CC4IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 1*2))) | (0<<( 1*2));
		break;
	case 8:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM1->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 6*2))) | (0<<( 6*2));
		break;
	case 9:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM3->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 10:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM15->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 11:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM15->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (0<<(15*2));
		break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	case 0:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM1->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 6*2))) | (0<<( 6*2));
		break;
	case 1:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 2:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM15->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 3:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM15->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (0<<(15*2));
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM3->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 5*2))) | (0<<( 5*2));
		break;
	case 1:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		TIM1->DIER&=~TIM_DIER_CC4IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (0<<(11*2));
		break;
	case 2:
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM2->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 1*2))) | (0<<( 1*2));
		break;
	case 3:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 4:
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM2->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 5:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM1->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (0<<(10*2));
		break;
#elif defined(PN_FGR4D)
	case 0:
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM2->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 1*2))) | (0<<( 1*2));
		break;
	case 1:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 2:
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM2->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 3:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM1->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (0<<(10*2));
		break;
#elif defined(PN_TR8B)
	case 0:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM3->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 5*2))) | (0<<( 5*2));
		break;
	case 1:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM3->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 4*2))) | (0<<( 4*2));
		break;
	case 2:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 3*2))) | (0<<( 3*2));
		break;
	case 3:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		TIM1->DIER&=~TIM_DIER_CC4IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (0<<(11*2));
		break;
	case 4:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM15->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 5:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM3->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (0<<( 0*2));
		break;
	case 6:
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM2->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 1*2))) | (0<<( 1*2));
		break;
	case 7:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM1->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (0<<(10*2));
		break;
#elif defined(PN_GMR)
	case 0:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk);
		TIM1->DIER&=~TIM_DIER_CC1IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 8*2))) | (0<<( 8*2));
		break;
	case 1:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		TIM1->DIER&=~TIM_DIER_CC4IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (0<<(11*2));
		break;
	case 2:
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		TIM2->DIER&=~TIM_DIER_CC3IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (0<<( 2*2));
		break;
	case 3:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 9*2))) | (0<<( 9*2));
		break;
#elif defined(PN_TMR)
	case 0:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		TIM1->DIER&=~TIM_DIER_CC2IE;
		if (!NoOutput)
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (0<<(9*2));
		break;
#endif
	}
	pNPTTIM_TimerCompareIRQHandlers[ChannelNb]=NULL;
	{
		unsigned char *pAreIRQPrioritiesHigh;

		pAreIRQPrioritiesHigh=&NPTTIM_AreIRQPrioritiesHigh[NPTTIM_PWMChannelToTimerNumber[ChannelNb]];
		// Lower the IRQ priority of the disabled channel
		(*pAreIRQPrioritiesHigh)&=~(1<<NPTTIM_PWMChannelToTimerChannelNumber[ChannelNb]);
		// Lower the IRQ priority of the timer if all channels have low priority
		if (*pAreIRQPrioritiesHigh==0)
			NVIC_SetPriority(NPTTIM_PWMChannelToTimerIRQNumber[ChannelNb],IRQ_PRI_MEDIUM);
	}
	SYS_EnableIRQs();
	return;
}

// Enable the channel of a timer and set it in frozen mode
void NPTTIM_EnableChannel(unsigned long ChannelNb,BOOL InitialOutputLevel,
	tNPTTIM_TimerCompareIRQHandler *pTimerCompareIRQHandler,BOOL IsIRQPriorityHigh,BOOL NoOutput)
{
	unsigned long InitialOC1M;
    
	InitialOC1M=(InitialOutputLevel)?5:4;
	pNPTTIM_TimerCompareIRQHandlers[ChannelNb]=pTimerCompareIRQHandler;
	{
		unsigned char *pAreIRQPrioritiesHigh;
		unsigned long TimerChannelNb;

		TimerChannelNb=NPTTIM_PWMChannelToTimerChannelNumber[ChannelNb];
		pAreIRQPrioritiesHigh=&NPTTIM_AreIRQPrioritiesHigh[NPTTIM_PWMChannelToTimerNumber[ChannelNb]];
		// Save the timer channel IRQ priority
		(*pAreIRQPrioritiesHigh)=(unsigned char)(((*pAreIRQPrioritiesHigh)&~(1<<TimerChannelNb)) | (IsIRQPriorityHigh<<TimerChannelNb));
		// Set the timer IRQ priority
		NVIC_SetPriority(NPTTIM_PWMChannelToTimerIRQNumber[ChannelNb],(*pAreIRQPrioritiesHigh)?IRQ_PRI_REALTIME:IRQ_PRI_MEDIUM);
	}
	SYS_DisableIRQs();
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8)
	case 0:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[11/8]=(GPIOA->AFR[11/8]&~(0xFUL<<((11%8)*4))) | (2<<((11%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (2UL<<(11*2));
		}
		break;
	case 1:
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOD->AFR[ 0/8]=(GPIOD->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (2<<(( 0%8)*4));
			GPIOD->MODER=(GPIOD->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 2:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 3/8]=(GPIOB->AFR[ 3/8]&~(0xFUL<<(( 3%8)*4))) | (1<<(( 3%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 3*2))) | (2UL<<( 3*2));
		}
		break;
	case 3:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 4/8]=(GPIOB->AFR[ 4/8]&~(0xFUL<<(( 4%8)*4))) | (1<<(( 4%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 4*2))) | (2UL<<( 4*2));
		}
		break;
	case 4:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 6/8]=(GPIOB->AFR[ 6/8]&~(0xFUL<<(( 6%8)*4))) | (1<<(( 6%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 6*2))) | (2UL<<( 6*2));
		}
		break;
	case 5:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 0/8]=(GPIOB->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (1<<(( 0%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 6:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (5<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 7:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[15/8]=(GPIOB->AFR[15/8]&~(0xFUL<<((15%8)*4))) | (5<<((15%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (2UL<<(15*2));
		}
		break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[11/8]=(GPIOA->AFR[11/8]&~(0xFUL<<((11%8)*4))) | (2<<((11%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (2UL<<(11*2));
		}
		break;
	case 1:
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOD->AFR[ 0/8]=(GPIOD->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (2<<(( 0%8)*4));
			GPIOD->MODER=(GPIOD->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 2:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 3/8]=(GPIOB->AFR[ 3/8]&~(0xFUL<<(( 3%8)*4))) | (1<<(( 3%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 3*2))) | (2UL<<( 3*2));
		}
		break;
	case 3:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 4/8]=(GPIOB->AFR[ 4/8]&~(0xFUL<<(( 4%8)*4))) | (1<<(( 4%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 4*2))) | (2UL<<( 4*2));
		}
		break;
	case 4:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 5/8]=(GPIOB->AFR[ 5/8]&~(0xFUL<<(( 5%8)*4))) | (1<<(( 5%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 5*2))) | (2UL<<( 5*2));
		}
		break;
	case 5:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 8/8]=(GPIOA->AFR[ 8/8]&~(0xFUL<<(( 8%8)*4))) | (2<<(( 8%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 8*2))) | (2UL<<( 8*2));
		}
		break;
	case 6:
		TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 4/8]=(GPIOA->AFR[ 4/8]&~(0xFUL<<(( 4%8)*4))) | (4<<(( 4%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 4*2))) | (2UL<<( 4*2));
		}
		break;
	case 7:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 1/8]=(GPIOB->AFR[ 1/8]&~(0xFUL<<(( 1%8)*4))) | (1<<((1%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 1*2))) | (2UL<<( 1*2));
		}
		break;
	case 8:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 6/8]=(GPIOB->AFR[ 6/8]&~(0xFUL<<(( 6%8)*4))) | (1<<(( 6%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 6*2))) | (2UL<<( 6*2));
		}
		break;
	case 9:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 0/8]=(GPIOB->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (1<<(( 0%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 10:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (5<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 11:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[15/8]=(GPIOB->AFR[15/8]&~(0xFUL<<((15%8)*4))) | (5<<((15%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (2UL<<(15*2));
		}
		break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	case 0:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 6/8]=(GPIOB->AFR[ 6/8]&~(0xFUL<<(( 6%8)*4))) | (1<<(( 6%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 6*2))) | (2UL<<( 6*2));
		}
		break;
	case 1:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 0/8]=(GPIOB->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (2<<(( 0%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 2:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (5<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 3:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[15/8]=(GPIOB->AFR[15/8]&~(0xFUL<<((15%8)*4))) | (5<<((15%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (2UL<<(15*2));
		}
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 5/8]=(GPIOB->AFR[ 5/8]&~(0xFUL<<(( 5%8)*4))) | (1<<(( 5%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 5*2))) | (2UL<<( 5*2));
		}
		break;
	case 1:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[11/8]=(GPIOA->AFR[11/8]&~(0xFUL<<((11%8)*4))) | (2<<((11%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (2UL<<(11*2));
		}
		break;
	case 2:
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 1/8]=(GPIOA->AFR[ 1/8]&~(0xFUL<<(( 1%8)*4))) | (2<<(( 1%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 1*2))) | (2UL<<( 1*2));
		}
		break;
	case 3:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 0/8]=(GPIOB->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (2<<(( 0%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 4:
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (2<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 5:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[10/8]=(GPIOA->AFR[10/8]&~(0xFUL<<((10%8)*4))) | (2<<((10%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (2UL<<(10*2));
		}
		break;
#elif defined(PN_FGR4D)
	case 0:
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 1/8]=(GPIOA->AFR[ 1/8]&~(0xFUL<<(( 1%8)*4))) | (2<<(( 1%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 1*2))) | (2UL<<( 1*2));
		}
		break;
	case 1:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 0/8]=(GPIOB->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (2<<(( 0%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 2:
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (2<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 3:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[10/8]=(GPIOA->AFR[10/8]&~(0xFUL<<((10%8)*4))) | (2<<((10%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (2UL<<(10*2));
		}
		break;
#elif defined(PN_TR8B)
	case 0:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 5/8]=(GPIOB->AFR[ 5/8]&~(0xFUL<<(( 5%8)*4))) | (1<<(( 5%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 5*2))) | (2UL<<( 5*2));
		}
		break;
	case 1:
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 4/8]=(GPIOB->AFR[ 4/8]&~(0xFUL<<(( 4%8)*4))) | (1<<(( 4%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 4*2))) | (2UL<<( 4*2));
		}
		break;
	case 2:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 3/8]=(GPIOB->AFR[ 3/8]&~(0xFUL<<(( 3%8)*4))) | (1<<(( 3%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 3*2))) | (2UL<<( 3*2));
		}
		break;
	case 3:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[11/8]=(GPIOA->AFR[11/8]&~(0xFUL<<((11%8)*4))) | (2<<((11%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (2UL<<(11*2));
		}
		break;
	case 4:
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (5<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 5:
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOB->AFR[ 0/8]=(GPIOB->AFR[ 0/8]&~(0xFUL<<(( 0%8)*4))) | (1<<(( 0%8)*4));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<( 0*2))) | (2UL<<( 0*2));
		}
		break;
	case 6:
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 1/8]=(GPIOA->AFR[ 1/8]&~(0xFUL<<(( 1%8)*4))) | (2<<(( 1%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 1*2))) | (2UL<<( 1*2));
		}
		break;
	case 7:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[10/8]=(GPIOA->AFR[10/8]&~(0xFUL<<((10%8)*4))) | (2<<((10%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (2UL<<(10*2));
		}
		break;
#elif defined(PN_GMR)
	case 0:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 8/8]=(GPIOA->AFR[ 8/8]&~(0xFUL<<(( 8%8)*4))) | (2<<(( 8%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 8*2))) | (2UL<<( 8*2));
		}
		break;
	case 1:
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[11/8]=(GPIOA->AFR[11/8]&~(0xFUL<<((11%8)*4))) | (2<<((11%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (2UL<<(11*2));
		}
		break;
	case 2:
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 2/8]=(GPIOA->AFR[ 2/8]&~(0xFUL<<(( 2%8)*4))) | (2<<(( 2%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 2*2))) | (2UL<<( 2*2));
		}
		break;
	case 3:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[ 9/8]=(GPIOA->AFR[ 9/8]&~(0xFUL<<(( 9%8)*4))) | (2<<(( 9%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<( 9*2))) | (2UL<<( 9*2));
		}
		break;
#elif defined(PN_TMR)
	case 0:
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		if (!NoOutput)
		{
			GPIOA->AFR[9/8]=(GPIOA->AFR[9/8]&~(0xFUL<<((9%8)*4))) | (2<<((9%8)*4));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (2UL<<(9*2));
		}
		break;
#endif
	default:
		SYS_EnableIRQs();
		return;
	}
	SYS_EnableIRQs();
	return;
}

void NPTTIM_FreezeCompareIRQ(unsigned long ChannelNb)
{
	SYS_DisableIRQs();
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8)
	case 0:
		TIM1->DIER&=~TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		break;
	case 1:
		TIM16->DIER&=~TIM_DIER_CC1IE;
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 2:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 3:
		TIM3->DIER&=~TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 4:
		TIM1->DIER&=~TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 5:
		TIM3->DIER&=~TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 6:
		TIM15->DIER&=~TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 7:
		TIM15->DIER&=~TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		TIM1->DIER&=~TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		break;
	case 1:
		TIM16->DIER&=~TIM_DIER_CC1IE;
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 2:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 3:
		TIM3->DIER&=~TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 4:
		TIM3->DIER&=~TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 5:
		TIM1->DIER&=~TIM_DIER_CC1IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 6:
		TIM14->DIER&=~TIM_DIER_CC1IE;
		TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 7:
		TIM3->DIER&=~TIM_DIER_CC4IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk);
		break;
	case 8:
		TIM1->DIER&=~TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 9:
		TIM3->DIER&=~TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 10:
		TIM15->DIER&=~TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 11:
		TIM15->DIER&=~TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	case 0:
		TIM1->DIER&=~TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 1:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 2:
		TIM15->DIER&=~TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 3:
		TIM15->DIER&=~TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0:
		TIM3->DIER&=~TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 1:
		TIM1->DIER&=~TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		break;
	case 2:
		TIM2->DIER&=~TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 3:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 4:
		TIM2->DIER&=~TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 5:
		TIM1->DIER&=~TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
#elif defined(PN_FGR4D)
	case 0:
		TIM2->DIER&=~TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 1:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 2:
		TIM2->DIER&=~TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 3:
		TIM1->DIER&=~TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
#elif defined(PN_TR8B)
	case 0:
		TIM3->DIER&=~TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 1:
		TIM3->DIER&=~TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 2:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 3:
		TIM1->DIER&=~TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		break;
	case 4:
		TIM15->DIER&=~TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 5:
		TIM3->DIER&=~TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 6:
		TIM2->DIER&=~TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
	case 7:
		TIM1->DIER&=~TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;

#elif defined(PN_GMR)
	case 0:
		TIM1->DIER&=~TIM_DIER_CC1IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk);
		break;
	case 1:
		TIM1->DIER&=~TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk);
		break;
	case 2:
		TIM2->DIER&=~TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk);
		break;
	case 3:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
#elif defined(PN_TMR)
	case 0:
		TIM1->DIER&=~TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk);
		break;
#endif
	default:
		break;
	}
	SYS_EnableIRQs();
	return;
}

unsigned short NPTTIM_GetCounter(unsigned long ChannelNb)
{
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8)
	case 0:
		return (unsigned short)TIM1->CNT;
	case 1:
		return (unsigned short)TIM16->CNT;
	case 2:
		return (unsigned short)TIM1->CNT;
	case 3:
		return (unsigned short)TIM3->CNT;
	case 4:
		return (unsigned short)TIM1->CNT;
	case 5:
		return (unsigned short)TIM3->CNT;
	case 6:
		return (unsigned short)TIM15->CNT;
	case 7:
		return (unsigned short)TIM15->CNT;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		return (unsigned short)TIM1->CNT;
	case 1:
		return (unsigned short)TIM16->CNT;
	case 2:
		return (unsigned short)TIM1->CNT;
	case 3:
		return (unsigned short)TIM3->CNT;
	case 4:
		return (unsigned short)TIM3->CNT;
	case 5:
		return (unsigned short)TIM1->CNT;
	case 6:
		return (unsigned short)TIM14->CNT;
	case 7:
		return (unsigned short)TIM3->CNT;
	case 8:
		return (unsigned short)TIM1->CNT;
	case 9:
		return (unsigned short)TIM3->CNT;
	case 10:
		return (unsigned short)TIM15->CNT;
	case 11:
		return (unsigned short)TIM15->CNT;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	case 0:
		return (unsigned short)TIM1->CNT;
	case 1:
		return (unsigned short)TIM1->CNT;
	case 2:
		return (unsigned short)TIM15->CNT;
	case 3:
		return (unsigned short)TIM15->CNT;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0: 
		return (unsigned short)TIM3->CNT;
	case 1:
		return (unsigned short)TIM1->CNT;
	case 2:
		return (unsigned short)TIM2->CNT;
	case 3:
		return (unsigned short)TIM1->CNT;
	case 4:
		return (unsigned short)TIM2->CNT;
	case 5:
		return (unsigned short)TIM1->CNT;
#elif defined(PN_FGR4D)
	case 0:
		return (unsigned short)TIM2->CNT;
	case 1:
		return (unsigned short)TIM1->CNT;
	case 2:
		return (unsigned short)TIM2->CNT;
	case 3:
		return (unsigned short)TIM1->CNT;
#elif defined(PN_TR8B)
	case 0:
		return (unsigned short)TIM3->CNT;
	case 1:
		return (unsigned short)TIM3->CNT;
	case 2:
		return (unsigned short)TIM1->CNT;
	case 3:
		return (unsigned short)TIM1->CNT;
	case 4:
		return (unsigned short)TIM15->CNT;
	case 5:
		return (unsigned short)TIM3->CNT;
	case 6:
		return (unsigned short)TIM2->CNT;
	case 7:
		return (unsigned short)TIM1->CNT;
#elif defined(PN_GMR)
	case 0:
		return (unsigned short)TIM1->CNT;
	case 1:
		return (unsigned short)TIM1->CNT;
	case 2:
		return (unsigned short)TIM2->CNT;
	case 3:
		return (unsigned short)TIM1->CNT;
#elif defined(PN_TMR)
	case 0:
		return (unsigned short)TIM1->CNT;
#endif
	default:
		return 0;
	}
}

void NPTTIM_Init(void)
{
	if (NPTTIM_Initialized)
		return;

	// Initialize all the required timers to 4MHz and enable the required timer outpouts
	// Set outputs to output compare freeze mode and compare value to zero to disable any IRQ (channel disabled)

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// Enable and reset TIM1
	RCC->APBENR2|=RCC_APBENR2_TIM1EN;
	RCC->APBRSTR2|=RCC_APBRSTR2_TIM1RST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_TIM1RST;
#elif defined(PN_GMR)
	// Enable and reset TIM1
	RCC->APB2ENR|=RCC_APB2ENR_TIM1EN;
	RCC->APB2RSTR|=RCC_APB2RSTR_TIM1RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_TIM1RST;
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_TR8B) || defined(PN_FBR8)
	// Enable channels 2, 3 and 4
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=1 Capture/Compare 1 output enabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	TIM1->CCER=TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set output compare values of channels 2, 3 and 4 to zero
	TIM1->CCR2=0;
	TIM1->CCR3=0;
	TIM1->CCR4=0;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	// Enable channels 1, 2, 3 and 4
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=0 Capture/Compare 1 output disabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=1 Capture/Compare 1 output enabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM1->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set output compare values of channels 1, 2, 3 and 4 to zero
	TIM1->CCR1=0;
	TIM1->CCR2=0;
	TIM1->CCR3=0;
	TIM1->CCR4=0;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	// Enable channels 2N and 3
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=0 Capture/Compare 1 output disabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=1 Capture/Compare 1 complementary output ensabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output disabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM1->CCER=TIM_CCER_CC2NE | TIM_CCER_CC3E;
	// Set output compare values of channels 2 and 3 to zero
	TIM1->CCR2=0;
	TIM1->CCR3=0;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC)
	// Enable channels 2N, 3 and 4
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=0 Capture/Compare 1 output disabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=1 Capture/Compare 1 complementary output ensabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=1 Capture/Compare 1 output enabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM1->CCER=TIM_CCER_CC2NE | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set output compare values of channels 2, 3 and 4 to zero
	TIM1->CCR2=0;
	TIM1->CCR3=0;
	TIM1->CCR4=0;
#elif defined(PN_FGR4D)
	// Enable channels 2N and 3
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=0 Capture/Compare 1 output disabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=1 Capture/Compare 1 complementary output ensabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output enabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM1->CCER=TIM_CCER_CC2NE | TIM_CCER_CC3E;
	// Set output compare values of channels 2 and 3 to zero
	TIM1->CCR2=0;
	TIM1->CCR3=0;
#elif defined(PN_GMR)
	// Enable channels 1, 2, and 4
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=1 Capture/Compare 1 output enabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM1->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set output compare values of channels 1, 2, 3 and 4 to zero
	TIM1->CCR1=0;
	TIM1->CCR2=0;
	TIM1->CCR3=0;
	TIM1->CCR4=0;
#elif defined(PN_TMR)
	// Enable channel 2
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=0 Capture/Compare 1 output disabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output disabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM1->CCER=TIM_CCER_CC2E;
	// Set output compare values of channel 2 to zero
	TIM1->CCR2=0;
#endif
	// Set a 4MHz counter
	TIM1->PSC=SYSCLK/4000000-1;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Output Compare 1 fast disbled
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Frozen
	// OC1CE=0 Output Compare 1 clear disabled
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Output Compare 2 fast disbled
	// OC2PE=0 Output Compare 2 preload disabled
	// OC2M=0 Frozen
	// OC2CE=0 Output Compare 2 clear disabled
	TIM1->CCMR1=(TIM_CCMR1_OC1M_0*0) | (TIM_CCMR1_OC2M_0*0);
	// CC3S=0 CC3 channel is configured as output
	// OC3FE=0 Output Compare 3 fast disbled
	// OC3PE=0 Output Compare 3 preload disabled
	// OC3M=0 Frozen
	// OC3CE=0 Output Compare 3 clear disabled
	// CC4S=0 CC4 channel is configured as output
	// OC4FE=0 Output Compare 4 fast disbled
	// OC4PE=0 Output Compare 4 preload disabled
	// OC4M=0 Frozen
	// OC4CE=0 Output Compare 4 clear disabled
	TIM1->CCMR2=(TIM_CCMR2_OC3M_0*0) | (TIM_CCMR2_OC4M_0*0);
	// Set a full 16-bit counter
	TIM1->ARR=0xFFFF;
	// Enable outputs
	TIM1->BDTR|=TIM_BDTR_MOE;
	// Set the timer TIM1 IRQ to medium priority
	NVIC_SetPriority(TIM1_CC_IRQn,IRQ_PRI_MEDIUM);
	// Enable the timer TIM1 IRQ
	NVIC_EnableIRQ(TIM1_CC_IRQn);
#endif

#if defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
#if defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	// Enable and reset TIM2
	RCC->APBENR1|=RCC_APBENR1_TIM2EN;
	RCC->APBRSTR1|=RCC_APBRSTR1_TIM2RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_TIM2RST;
#elif defined(PN_GMR)
	// Enable and reset TIM2
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	RCC->APB1RSTR|=RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_TIM2RST;
#endif
#if defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	// Enable channels 2 and 3
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output disabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM2->CCER=TIM_CCER_CC2E | TIM_CCER_CC3E;
	// Set output compare values of channels 2, 3 and 4 to zero
	TIM2->CCR2=0;
	TIM2->CCR3=0;
#elif defined(PN_TR8B)
	// Enable channels 2
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	TIM2->CCER=TIM_CCER_CC2E;
	// Set output compare values of channels 2 to zero
	TIM2->CCR2=0;
#elif defined(PN_GMR)
	// Enable channel 3
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=0 Capture/Compare 1 output disabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NE=0 Capture/Compare 1 complementary output disabled
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NE=0 Capture/Compare 1 complementary output disabled
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output disabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	// CC5E=0 Capture/Compare 1 output disabled
	// CC5P=0 Capture/Compare 1 output active high
	// CC6E=0 Capture/Compare 1 output disabled
	// CC6P=0 Capture/Compare 1 output active high
	TIM2->CCER=TIM_CCER_CC3E;
	// Set output compare values of channels 3 to zero
	TIM2->CCR3=0;
#endif
	// Set a 4MHz counter
	TIM2->PSC=SYSCLK/4000000-1;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Output Compare 1 fast disbled
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Frozen
	// OC1CE=0 Output Compare 1 clear disabled
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Output Compare 2 fast disbled
	// OC2PE=0 Output Compare 2 preload disabled
	// OC2M=0 Frozen
	// OC2CE=0 Output Compare 2 clear disabled
	TIM2->CCMR1=(TIM_CCMR1_OC1M_0*0) | (TIM_CCMR1_OC2M_0*0);
	// CC3S=0 CC3 channel is configured as output
	// OC3FE=0 Output Compare 3 fast disbled
	// OC3PE=0 Output Compare 3 preload disabled
	// OC3M=0 Frozen
	// OC3CE=0 Output Compare 3 clear disabled
	// CC4S=0 CC4 channel is configured as output
	// OC4FE=0 Output Compare 4 fast disbled
	// OC4PE=0 Output Compare 4 preload disabled
	// OC4M=0 Frozen
	// OC4CE=0 Output Compare 4 clear disabled
	TIM2->CCMR2=(TIM_CCMR2_OC3M_0*0) | (TIM_CCMR2_OC4M_0*0);
	// Set a full 16-bit counter
	TIM2->ARR=0xFFFF;
	// Enable outputs
	TIM2->BDTR|=TIM_BDTR_MOE;
	// Set the timer TIM2 IRQ to medium priority
	NVIC_SetPriority(TIM2_IRQn,IRQ_PRI_MEDIUM);
	// Enable the timer TIM2 IRQ
	NVIC_EnableIRQ(TIM2_IRQn);
#endif

#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B)\
	|| defined(PN_FBR8) 
	// Enable and reset TIM3
	RCC->APBENR1|=RCC_APBENR1_TIM3EN;
	RCC->APBRSTR1|=RCC_APBRSTR1_TIM3RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_TIM3RST;
#if defined(PN_FTR8B) || defined(PN_FGR8B)|| defined(PN_FBR8)
	// Enable channels 1 and 3
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=0 Capture/Compare 1 output disabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output disabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	TIM3->CCER=TIM_CCER_CC1E | TIM_CCER_CC3E;
	// Set output compare values of channels 1, 2 and 3 to zero
	TIM3->CCR1=0;
	TIM3->CCR3=0;
	// Enable outputs
	TIM3->BDTR|=TIM_BDTR_MOE;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	// Enable channels 1, 2, 3 and 4
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=1 Capture/Compare 1 output enabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	TIM3->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set output compare values of channels 1, 2, 3 and 4 to zero
	TIM3->CCR1=0;
	TIM3->CCR2=0;
	TIM3->CCR3=0;
	TIM3->CCR4=0;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	// Enable channel 2
	// CC1E=0 Capture/Compare 1 output disabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=0 Capture/Compare 1 output disabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NP=0 Capture/Compare 1 complementary output active high
	// CC4E=0 Capture/Compare 1 output disabled
	// CC4P=0 Capture/Compare 1 output active high
	// CC4NP=0 Capture/Compare 1 complementary output active high
	TIM3->CCER=TIM_CCER_CC2E;
	// Set output compare values of channels 1, 2, 3 and 4 to zero
	TIM3->CCR2=0;
#elif defined(PN_TR8B)
// Enable channels 1, 2, 3
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NP=0 Capture/Compare 1 complementary output active high
	// CC3E=1 Capture/Compare 1 output enabled
	// CC3P=0 Capture/Compare 1 output active high
	// CC3NP=0 Capture/Compare 1 complementary output active high
	TIM3->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;
	// Set output compare values of channels 1, 2, 3 to zero
	TIM3->CCR1=0;
	TIM3->CCR2=0;
	TIM3->CCR3=0;
#endif
	// Set a 4MHz counter
	TIM3->PSC=SYSCLK/4000000-1;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Output Compare 1 fast disbled
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Frozen
	// OC1CE=0 Output Compare 1 clear disabled
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Output Compare 2 fast disbled
	// OC2PE=0 Output Compare 2 preload disabled
	// OC2M=0 Frozen
	// OC2CE=0 Output Compare 2 clear disabled
	TIM3->CCMR1=(TIM_CCMR1_OC1M_0*0) | (TIM_CCMR1_OC2M_0*0);
	// CC3S=0 CC3 channel is configured as output
	// OC3FE=0 Output Compare 3 fast disbled
	// OC3PE=0 Output Compare 3 preload disabled
	// OC3M=0 Frozen
	// OC3CE=0 Output Compare 3 clear disabled
	// CC4S=0 CC4 channel is configured as output
	// OC4FE=0 Output Compare 4 fast disbled
	// OC4PE=0 Output Compare 4 preload disabled
	// OC4M=0 Frozen
	// OC4CE=0 Output Compare 4 clear disabled
	TIM3->CCMR2=(TIM_CCMR2_OC3M_0*0) | (TIM_CCMR2_OC4M_0*0);
	// Set a full 16-bit counter
	TIM3->ARR=0xFFFF;
	// Enable outputs
	TIM3->BDTR|=TIM_BDTR_MOE;
	// Set the timer TIM3 IRQ to medium priority
	NVIC_SetPriority(TIM3_IRQn,IRQ_PRI_MEDIUM);
	// Enable the timer TIM3 IRQ
	NVIC_EnableIRQ(TIM3_IRQn);
#endif

#if defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	// Enable and reset TIM14
	RCC->APBENR2|=RCC_APBENR2_TIM14EN;
	RCC->APBRSTR2|=RCC_APBRSTR2_TIM14RST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_TIM14RST;
	// Enable the channel 1
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 OC1N output not active
	// CC1NP=0 Not used
	TIM14->CCER=TIM_CCER_CC1E;
	// Set output compare values of channel 1 to zero
	TIM14->CCR1=0;
	// Set a 4MHz counter
	TIM14->PSC=SYSCLK/4000000-1;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Output Compare 1 fast disbled
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Frozen
	TIM14->CCMR1=(TIM_CCMR1_OC1M_0*0);
	// Set a full 16-bit counter
	TIM14->ARR=0xFFFF;
	// Enable outputs
	TIM14->BDTR|=TIM_BDTR_MOE;
	// Set the timer TIM14 IRQ to medium priority
	NVIC_SetPriority(TIM14_IRQn,IRQ_PRI_MEDIUM);
	// Enable the timer TIM14 IRQ
	NVIC_EnableIRQ(TIM14_IRQn);
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4)
	// Enable and reset TIM15
	RCC->APBENR2|=RCC_APBENR2_TIM15EN;
	RCC->APBRSTR2|=RCC_APBRSTR2_TIM15RST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_TIM15RST;
#if defined(PN_TR8B)
	// Enable the channels 1
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	TIM15->CCER=TIM_CCER_CC1E;
	// Set output compare values of channels 1 to zero
	TIM15->CCR1=0;
#else
	// Enable the channels 1 and 2
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 Capture/Compare 1 complementary output disabled
	// CC1NP=0 Capture/Compare 1 complementary output active high
	// CC2E=1 Capture/Compare 1 output enabled
	// CC2P=0 Capture/Compare 1 output active high
	// CC2NP=0 Capture/Compare 1 complementary output active high
	TIM15->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E;
	// Set output compare values of channels 1 and 2 to zero
	TIM15->CCR1=0;
	TIM15->CCR2=0;
#endif
	// Set a 4MHz counter
	TIM15->PSC=SYSCLK/4000000-1;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Output Compare 1 fast disbled
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Frozen
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Output Compare 2 fast disbled
	// OC2PE=0 Output Compare 2 preload disabled
	// OC2M=0 Frozen
	TIM15->CCMR1=(TIM_CCMR1_OC1M_0*0) | (TIM_CCMR1_OC2M_0*0);
	// Set a full 16-bit counter
	TIM15->ARR=0xFFFF;
	// Enable outputs
	TIM15->BDTR|=TIM_BDTR_MOE;
	// Set the timer TIM15 IRQ to medium priority
	NVIC_SetPriority(TIM15_IRQn,IRQ_PRI_MEDIUM);
	// Enable the timer TIM15 IRQ
	NVIC_EnableIRQ(TIM15_IRQn);
#endif

#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_FBR8)
	// Enable and reset TIM16
	RCC->APBENR2|=RCC_APBENR2_TIM16EN;
	RCC->APBRSTR2|=RCC_APBRSTR2_TIM16RST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_TIM16RST;
	// Enable the channel 1
	// CC1E=1 Capture/Compare 1 output enabled
	// CC1P=0 Capture/Compare 1 output active high
	// CC1NE=0 OC1N output not active
	// CC1NP=0 Not used
	TIM16->CCER=TIM_CCER_CC1E;
	// Set output compare values of channel 1 to zero
	TIM16->CCR1=0;
	// Set a 4MHz counter
	TIM16->PSC=SYSCLK/4000000-1;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Output Compare 1 fast disbled
	// OC1PE=0 Output Compare 1 preload disabled
	// OC1M=0 Frozen
	TIM16->CCMR1=(TIM_CCMR1_OC1M_0*0);
	// Set a full 16-bit counter
	TIM16->ARR=0xFFFF;
	// Enable outputs
	TIM16->BDTR|=TIM_BDTR_MOE;
	// Set the timer TIM16 IRQ to medium priority
	NVIC_SetPriority(TIM16_IRQn,IRQ_PRI_MEDIUM);
	// Enable the timer TIM16 IRQ
	NVIC_EnableIRQ(TIM16_IRQn);
	TIM16->CR1=TIM_CR1_CEN;
#endif

	// Set the IRQ priority of all channels of all timer to low
	memset(NPTTIM_AreIRQPrioritiesHigh,0,sizeof(NPTTIM_AreIRQPrioritiesHigh));
	
	// Synchronized start of all timers
	SYS_DisableIRQs();
	// Reinitialize the counter and generates an update of the registers
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	TIM1->EGR=TIM_EGR_UG;
#endif
#if defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	TIM2->EGR=TIM_EGR_UG;
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B)\
	|| defined(PN_FBR8) 
	TIM3->EGR=TIM_EGR_UG;
#endif
#if defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	TIM14->EGR=TIM_EGR_UG;
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4)
	TIM15->EGR=TIM_EGR_UG;
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)|| defined(PN_FBR8)
	TIM16->EGR=TIM_EGR_UG;
#endif

	// Enable the counter of the timer and keep the defaults
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	TIM1->CR1=TIM_CR1_CEN;
#endif
#if defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	TIM2->CR1=TIM_CR1_CEN;
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B)\
	|| defined(PN_FBR8) 
	TIM3->CR1=TIM_CR1_CEN;
#endif
#if defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	TIM14->CR1=TIM_CR1_CEN;
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4)
	TIM15->CR1=TIM_CR1_CEN;
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_FBR8)
	TIM16->CR1=TIM_CR1_CEN;
#endif
	SYS_EnableIRQs();

	NPTTIM_Initialized=TRUE;
	return;
}

void NPTTIM_SetNextCompareIRQCounter(unsigned long ChannelNb,BOOL InitialOutputLevel,unsigned long CounterValue,BOOL NextCompareIRQOutputLevel)
{
	unsigned long InitialOC1M;

	NPTTIM_TimerCounters[ChannelNb]=CounterValue;
	NPTTIM_RemainingTimes[ChannelNb]=0;
	NPTTIM_NextCompareIRQsOutputLevel[ChannelNb]=NextCompareIRQOutputLevel;
	SYS_DisableIRQs();
	InitialOC1M=(InitialOutputLevel)?5:4;
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8)
	case 0:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		break;
	case 1:
		TIM16->SR=~TIM_SR_CC1IF;
		TIM16->DIER|=TIM_DIER_CC1IE;
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 3:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 4:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 5:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 6:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 7:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		break;
	case 1:
		TIM16->SR=~TIM_SR_CC1IF;
		TIM16->DIER|=TIM_DIER_CC1IE;
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 3:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 4:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 5:
		TIM1->SR=~TIM_SR_CC1IF;
		TIM1->DIER|=TIM_DIER_CC1IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 6:
		TIM14->SR=~TIM_SR_CC1IF;
		TIM14->DIER|=TIM_DIER_CC1IE;
		TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 7:
		TIM3->SR=~TIM_SR_CC4IF;
		TIM3->DIER|=TIM_DIER_CC4IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		break;
	case 8:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 9:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 10:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 11:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB)|| defined(PN_FBR4)
	case 0:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 2:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 3:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 4:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 5:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
#elif defined(PN_FGR4D)
	case 0:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
#elif defined(PN_TR8B)
	case 0:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 1:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		break;
	case 4:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 5:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 6:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
	case 7:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
#elif defined(PN_GMR)
	case 0:
		TIM1->SR=~TIM_SR_CC1IF;
		TIM1->DIER|=TIM_DIER_CC1IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
#elif defined(PN_TMR)
	case 0:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		break;
#endif
	default:
		SYS_EnableIRQs();
		return;
	}
	SYS_EnableIRQs();
	NPTTIM_SetNextCompareIRQTimeInternal(ChannelNb);
	return;
}

void NPTTIM_SetNextCompareIRQTime(unsigned long ChannelNb,unsigned long Time,BOOL OutputLevel)
{
	NPTTIM_RemainingTimes[ChannelNb]=Time;
	NPTTIM_NextCompareIRQsOutputLevel[ChannelNb]=OutputLevel;
	NPTTIM_SetNextCompareIRQTimeInternal(ChannelNb);
	return;
}

// The IRQ compare must be frozen before calling this function
void NPTTIM_SetNextCompareIRQTimeFromNow(unsigned long ChannelNb,BOOL InitialOutputLevel,unsigned long Time,BOOL NextCompareIRQOutputLevel)
{
	volatile uint32_t *pCNT;
	unsigned long InitialOC1M;

	NPTTIM_RemainingTimes[ChannelNb]=Time;
	NPTTIM_NextCompareIRQsOutputLevel[ChannelNb]=NextCompareIRQOutputLevel;
	InitialOC1M=(InitialOutputLevel)?5:4;
	SYS_DisableIRQs();
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8) 
	case 0:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 1:
		TIM16->SR=~TIM_SR_CC1IF;
		TIM16->DIER|=TIM_DIER_CC1IE;
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM16->CNT;
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 3:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 4:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 5:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 6:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM15->CNT;
		break;
	case 7:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM15->CNT;
		break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 1:
		TIM16->SR=~TIM_SR_CC1IF;
		TIM16->DIER|=TIM_DIER_CC1IE;
		TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM16->CNT;
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 3:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 4:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 5:
		TIM1->SR=~TIM_SR_CC1IF;
		TIM1->DIER|=TIM_DIER_CC1IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 6:
		TIM14->SR=~TIM_SR_CC1IF;
		TIM14->DIER|=TIM_DIER_CC1IE;
		TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM14->CNT;
		break;
	case 7:
		TIM3->SR=~TIM_SR_CC4IF;
		TIM3->DIER|=TIM_DIER_CC4IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 8:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 9:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 10:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM15->CNT;
		break;
	case 11:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM15->CNT;
		break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB)|| defined(PN_FBR4)
	case 0:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 2:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM15->CNT;
		break;
	case 3:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM15->CNT;
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM2->CNT;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 4:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM2->CNT;
		break;
	case 5:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM1->CNT;
		break;
#elif defined(PN_FGR4D)
	case 0:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM2->CNT;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM2->CNT;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM1->CNT;
		break;
#elif defined(PN_TR8B)
	case 0:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 1:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 4:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM15->CNT;
		break;
	case 5:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM3->CNT;
		break;
	case 6:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM2->CNT;
		break;
	case 7:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM1->CNT;
		break;
#elif defined(PN_GMR)
	case 0:
		TIM1->SR=~TIM_SR_CC1IF;
		TIM1->DIER|=TIM_DIER_CC1IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (InitialOC1M<<TIM_CCMR1_OC1M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (InitialOC1M<<TIM_CCMR2_OC4M_Pos);
		pCNT=&TIM1->CNT;
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (InitialOC1M<<TIM_CCMR2_OC3M_Pos);
		pCNT=&TIM2->CNT;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
#elif defined(PN_TMR)
	case 0:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (InitialOC1M<<TIM_CCMR1_OC2M_Pos);
		pCNT=&TIM1->CNT;
		break;
#endif
	default:
		SYS_EnableIRQs();
		return;
	}
	NPTTIM_TimerCounters[ChannelNb]=*pCNT;
	NPTTIM_SetNextCompareIRQTimeInternal(ChannelNb);
	SYS_EnableIRQs();
	return;
}

void NPTTIM_SetNextCompareIRQTimeInternal(unsigned long ChannelNb)
{
	unsigned long RemainingTime;
	unsigned long TimerCounter;

	SYS_DisableIRQs();
	{
		RemainingTime=NPTTIM_RemainingTimes[ChannelNb];
		TimerCounter=NPTTIM_TimerCounters[ChannelNb];
		if (RemainingTime<=0x10000)
		{
			TimerCounter=(TimerCounter+RemainingTime)&0xFFFF;
			RemainingTime=0;
		}
		else if (RemainingTime<=0x20000)
		{
			unsigned long NextTime;
			
			NextTime=RemainingTime/2;
			TimerCounter=(TimerCounter+NextTime)&0xFFFF;
			RemainingTime-=NextTime;
		}
		else
			RemainingTime-=0x10000;
		NPTTIM_RemainingTimes[ChannelNb]=RemainingTime;
		NPTTIM_TimerCounters[ChannelNb]=TimerCounter;
	}
	{
		unsigned long OC1M;

		OC1M=(!RemainingTime)?((NPTTIM_NextCompareIRQsOutputLevel[ChannelNb])?1:2):0;
		switch (ChannelNb)
		{
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8) 
		case 0:
			TIM1->CCR4=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (OC1M<<TIM_CCMR2_OC4M_Pos);
			break;
		case 1:
			TIM16->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 2:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 3:
			TIM3->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 4:
			TIM1->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 5:
			TIM3->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 6:
			TIM15->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 7:
			TIM15->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
		case 0:
			TIM1->CCR4=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (OC1M<<TIM_CCMR2_OC4M_Pos);
			break;
		case 1:
			TIM16->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM16->CCMR1=(TIM16->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 2:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 3:
			TIM3->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 4:
			TIM3->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 5:
			TIM1->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 6:
			TIM14->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM14->CCMR1=(TIM14->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 7:
			TIM3->CCR4=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC4M_Msk) | (OC1M<<TIM_CCMR2_OC4M_Pos);
			break;
		case 8:
			TIM1->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 9:
			TIM3->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 10:
			TIM15->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 11:
			TIM15->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
		case 0:
			TIM1->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 1:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 2:
			TIM15->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 3:
			TIM15->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC)
		case 0:
			TIM3->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 1:
			TIM1->CCR4=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (OC1M<<TIM_CCMR2_OC4M_Pos);
			break;
		case 2:
			TIM2->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 3:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 4:
			TIM2->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 5:
			TIM1->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
#elif defined(PN_FGR4D)
		case 0:
			TIM2->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 1:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 2:
			TIM2->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 3:
			TIM1->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
#elif defined(PN_TR8B)
		case 0:
			TIM3->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 1:
			TIM3->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR1=(TIM3->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 2:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 3:
			TIM1->CCR4=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (OC1M<<TIM_CCMR2_OC4M_Pos);
			break;
		case 4:
			TIM15->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM15->CCMR1=(TIM15->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 5:
			TIM3->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM3->CCMR2=(TIM3->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 6:
			TIM2->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM2->CCMR1=(TIM2->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
		case 7:
			TIM1->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
#elif defined(PN_GMR)
		case 0:
			TIM1->CCR1=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC1M_Msk) | (OC1M<<TIM_CCMR1_OC1M_Pos);
			break;
		case 1:
			TIM1->CCR4=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR2=(TIM1->CCMR2&~TIM_CCMR2_OC4M_Msk) | (OC1M<<TIM_CCMR2_OC4M_Pos);
			break;
		case 2:
			TIM2->CCR3=TimerCounter;
			if (!RemainingTime)
				TIM2->CCMR2=(TIM2->CCMR2&~TIM_CCMR2_OC3M_Msk) | (OC1M<<TIM_CCMR2_OC3M_Pos);
			break;
		case 3:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
#elif defined(PN_TMR)
		case 0:
			TIM1->CCR2=TimerCounter;
			if (!RemainingTime)
				TIM1->CCMR1=(TIM1->CCMR1&~TIM_CCMR1_OC2M_Msk) | (OC1M<<TIM_CCMR1_OC2M_Pos);
			break;
#endif
		}
	}
	SYS_EnableIRQs();
	return;
}

void NPTTIM_TriggerIRQ(unsigned long ChannelNb)
{
	SYS_DisableIRQs();
	NPTTIM_TimerCounters[ChannelNb]=0;
	switch (ChannelNb)
	{
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FBR8)
	case 0:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->EGR=TIM_EGR_CC4G;
		break;
	case 1:
		TIM16->SR=~TIM_SR_CC1IF;
		TIM16->DIER|=TIM_DIER_CC1IE;
		TIM16->EGR=TIM_EGR_CC1G;
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
	case 3:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->EGR=TIM_EGR_CC1G;
		break;
	case 4:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->EGR=TIM_EGR_CC3G;
		break;
	case 5:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->EGR=TIM_EGR_CC3G;
		break;
	case 6:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->EGR=TIM_EGR_CC1G;
		break;
	case 7:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->EGR=TIM_EGR_CC2G;
		break;
#elif defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	case 0:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->EGR=TIM_EGR_CC4G;
		break;
	case 1:
		TIM16->SR=~TIM_SR_CC1IF;
		TIM16->DIER|=TIM_DIER_CC1IE;
		TIM16->EGR=TIM_EGR_CC1G;
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
	case 3:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->EGR=TIM_EGR_CC1G;
		break;
	case 4:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->EGR=TIM_EGR_CC2G;
		break;
	case 5:
		TIM1->SR=~TIM_SR_CC1IF;
		TIM1->DIER|=TIM_DIER_CC1IE;
		TIM1->EGR=TIM_EGR_CC1G;
		break;
	case 6:
		TIM14->SR=~TIM_SR_CC1IF;
		TIM14->DIER|=TIM_DIER_CC1IE;
		TIM14->EGR=TIM_EGR_CC1G;
		break;
	case 7:
		TIM3->SR=~TIM_SR_CC4IF;
		TIM3->DIER|=TIM_DIER_CC4IE;
		TIM3->EGR=TIM_EGR_CC4G;
		break;
	case 8:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->EGR=TIM_EGR_CC3G;
		break;
	case 9:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->EGR=TIM_EGR_CC3G;
		break;
	case 10:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->EGR=TIM_EGR_CC1G;
		break;
	case 11:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->EGR=TIM_EGR_CC2G;
		break;
#elif defined(PN_FGR4B) || defined(PN_INR4_GYB) || defined(PN_FBR4)
	case 0:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->EGR=TIM_EGR_CC3G;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
	case 2:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->EGR=TIM_EGR_CC1G;
		break;
	case 3:
		TIM15->SR=~TIM_SR_CC2IF;
		TIM15->DIER|=TIM_DIER_CC2IE;
		TIM15->EGR=TIM_EGR_CC2G;
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) 
	case 0:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->EGR=TIM_EGR_CC2G;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->EGR=TIM_EGR_CC4G;
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->EGR=TIM_EGR_CC2G;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
	case 4:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->EGR=TIM_EGR_CC3G;
		break;
	case 5:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->EGR=TIM_EGR_CC3G;
		break;
#elif defined(PN_FGR4D)
	case 0:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->EGR=TIM_EGR_CC2G;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->EGR=TIM_EGR_CC3G;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->EGR=TIM_EGR_CC3G;
		break;
#elif defined(PN_TR8B)
	case 0:
		TIM3->SR=~TIM_SR_CC2IF;
		TIM3->DIER|=TIM_DIER_CC2IE;
		TIM3->EGR=TIM_EGR_CC2G;
		break;
	case 1:
		TIM3->SR=~TIM_SR_CC1IF;
		TIM3->DIER|=TIM_DIER_CC1IE;
		TIM3->EGR=TIM_EGR_CC1G;
		break;
	case 2:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->EGR=TIM_EGR_CC4G;
		break;
	case 4:
		TIM15->SR=~TIM_SR_CC1IF;
		TIM15->DIER|=TIM_DIER_CC1IE;
		TIM15->EGR=TIM_EGR_CC1G;
		break;
	case 5:
		TIM3->SR=~TIM_SR_CC3IF;
		TIM3->DIER|=TIM_DIER_CC3IE;
		TIM3->EGR=TIM_EGR_CC3G;
		break;
	case 6:
		TIM2->SR=~TIM_SR_CC2IF;
		TIM2->DIER|=TIM_DIER_CC2IE;
		TIM2->EGR=TIM_EGR_CC2G;
		break;
	case 7:
		TIM1->SR=~TIM_SR_CC3IF;
		TIM1->DIER|=TIM_DIER_CC3IE;
		TIM1->EGR=TIM_EGR_CC3G;
		break;
#elif defined(PN_GMR)
	case 0:
		TIM1->SR=~TIM_SR_CC1IF;
		TIM1->DIER|=TIM_DIER_CC1IE;
		TIM1->EGR=TIM_EGR_CC1G;
		break;
	case 1:
		TIM1->SR=~TIM_SR_CC4IF;
		TIM1->DIER|=TIM_DIER_CC4IE;
		TIM1->EGR=TIM_EGR_CC4G;
		break;
	case 2:
		TIM2->SR=~TIM_SR_CC3IF;
		TIM2->DIER|=TIM_DIER_CC3IE;
		TIM2->EGR=TIM_EGR_CC3G;
		break;
	case 3:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
#elif defined(PN_TMR)
	case 0:
		TIM1->SR=~TIM_SR_CC2IF;
		TIM1->DIER|=TIM_DIER_CC2IE;
		TIM1->EGR=TIM_EGR_CC2G;
		break;
#endif
	}
	SYS_EnableIRQs();
	return;
}
