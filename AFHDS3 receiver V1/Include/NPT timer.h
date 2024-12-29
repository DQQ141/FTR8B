#if !defined(NPT_TIMER_H)
#define NPT_TIMER_H

#include "System definitions.h"

typedef void tNPTTIM_TimerCompareIRQHandler(unsigned long ChannelNb,unsigned char AreIRQPrioritiesHigh);

void NPTTIM_DisableChannel(unsigned long ChannelNb,BOOL NoOutput);
void NPTTIM_EnableChannel(unsigned long ChannelNb,BOOL InitialOutputLevel,
	tNPTTIM_TimerCompareIRQHandler *pTimerCompareIRQHandler,BOOL IsIRQPriorityHigh,BOOL NoOutput);
void NPTTIM_FreezeCompareIRQ(unsigned long ChannelNb);
void NPTTIM_Init(void);
void NPTTIM_SetNextCompareIRQTime(unsigned long ChannelNb,unsigned long Time,BOOL OutputLevel);
// The IRQ compare must be frozen before calling this function
void NPTTIM_SetNextCompareIRQTimeFromNow(unsigned long ChannelNb,BOOL InitialOutputLevel,unsigned long Time,BOOL NextCompareIRQOutputLevel);
void NPTTIM_SetNextCompareIRQTimeInternal(unsigned long ChannelNb);
void NPTTIM_TriggerIRQ(unsigned long ChannelNb);
void NPTTIM_SetNextCompareIRQCounter(unsigned long ChannelNb,BOOL InitialOutputLevel,unsigned long CounterValue,BOOL NextCompareIRQOutputLevel);
unsigned short NPTTIM_GetCounter(unsigned long ChannelNb);

#endif // !defined(NPT_TIMER_H)
