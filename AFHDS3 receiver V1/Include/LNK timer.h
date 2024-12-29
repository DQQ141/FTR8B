#if !defined(LNK_TIMER_H)
#define LNK_TIMER_H

#include "System definitions.h"

typedef void tLNKTIM_TimerCompareIRQHandler(void);

#if defined(BOOTLOADER)
	void TIM17_IRQHandler2(void);
#else
	void TIM17_IRQHandler(void);
#endif
unsigned short LNKTIM_GetCounter(void);
void LNKTIM_Init(tLNKTIM_TimerCompareIRQHandler *pTimerCompareIRQHandler);
void LNKTIM_SetNextCompareIRQTime(unsigned long Time);
void LNKTIM_SetNextCompareIRQTimeFromMarker(unsigned long Time);
void LNKTIM_SetNextCompareIRQTimeInternal(void);
void LNKTIM_SetTimeMarker(void);
void LNKTIM_StartTimer(unsigned long FirstCompareIRQTime);
void LNKTIM_StopTimer(void);

#endif // !defined(LNK_TIMER_H)
