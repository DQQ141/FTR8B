#if !defined(NPT_TIMER_PRIVATE_H)
#define NPT_TIMER_PRIVATE_H

#include "System definitions.h"

#define NPTTIM_TIMER_NB_MAX_CHANNELS 4

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	void TIM1_CC_IRQHandler(void);
#endif
#if defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	void TIM2_IRQHandler(void);
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) ||defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B)\
	|| defined(PN_FBR8) || defined(PN_FGR4D)
	void TIM3_IRQHandler(void);
#endif
#if defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
	void TIM14_IRQHandler(void);
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4)
	void TIM15_IRQHandler(void);
#endif
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)|| defined(PN_FBR8) 
	void TIM16_IRQHandler(void);
#endif

#endif // !defined(NPT_TIMER_PRIVATE_H)
