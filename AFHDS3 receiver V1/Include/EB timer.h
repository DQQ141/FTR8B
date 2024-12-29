#if !defined(EB_TIMER_H)
#define EB_TIMER_H

#include "System definitions.h"

typedef void tEBTIM_TimerCompareIRQHandler(void);

void TIM7_IRQHandler(void);
void EBTIM_DeInit(void);
void EBTIM_Init(tEBTIM_TimerCompareIRQHandler *pTimerCompareIRQHandler);
void EBTIM_SetNextCompareIRQTime(unsigned long Time);
void EBTIM_StartTimer(unsigned long FirstCompareIRQTime);
void EBTIM_StopTimer(void);

#endif // !defined(EB_TIMER_H)
