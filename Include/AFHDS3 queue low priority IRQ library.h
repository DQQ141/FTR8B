#if !defined(AFHDS3_QUEUE_LOW_PRIORITY_IRQ_LIBRARY_H)
#define AFHDS3_QUEUE_LOW_PRIORITY_IRQ_LIBRARY_H

#include "System definitions.h"

typedef void tSYS_LowPriorityIRQCallback(unsigned long CallbackValue);

void SYS_LowPriorityIRQCallback(void);
void SYS_QueueLowPriorityIRQCallback(tSYS_LowPriorityIRQCallback *pCallback,unsigned long CallbackValue);
#if defined(SYS_NB_LOW_PRIORITY_IRQ_CALLBACK_LAST)
	void SYS_QueueLowPriorityIRQCallbackLast(tSYS_LowPriorityIRQCallback *pCallback,unsigned long CallbackValue);
#endif

void SYS_DisableIRQs(void);
void SYS_EnableIRQs(void);
void SYS_TriggerLowPriorityCallback(void);

#endif // !defined(AFHDS3_QUEUE_LOW_PRIORITY_IRQ_LIBRARY_H)
