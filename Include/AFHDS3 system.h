#if !defined(AFHDS3_SYSTEM_H)
#define AFHDS3_SYSTEM_H

#include "System definitions.h"

extern volatile unsigned long SYS_SysTickMs;

extern volatile BOOL SYS_InLowPriorityCallback;

void SYS_DisableIRQs(void);
void SYS_EnableIRQs(void);

#endif // !defined(AFHDS3_SYSTEM_H)
