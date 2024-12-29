#if !defined(AFHDS3_QUEUE_MEDIUM_PRIORITY_IRQ_LIBRARY_H)
#define AFHDS3_QUEUE_MEDIUM_PRIORITY_IRQ_LIBRARY_H

#include "System definitions.h"

typedef void tSYS_MediumPriorityIRQCallback(unsigned long CallbackValue);

void SYS_MediumPriorityIRQCallback(void);
void SYS_QueueMediumPriorityIRQCallback(tSYS_MediumPriorityIRQCallback *pCallback,unsigned long CallbackValue);
#if defined(SYS_NB_MEDIUM_PRIORITY_IRQ_CALLBACK_LAST)
	void SYS_QueueMediumPriorityIRQCallbackLast(tSYS_MediumPriorityIRQCallback *pCallback,unsigned long CallbackValue);
#endif

void SYS_DisableIRQs(void);
void SYS_EnableIRQs(void);
void SYS_TriggerMediumPriorityCallback(void);

#endif // !defined(AFHDS3_QUEUE_MEDIUM_PRIORITY_IRQ_LIBRARY_H)
