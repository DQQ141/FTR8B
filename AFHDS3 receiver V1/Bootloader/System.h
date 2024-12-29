#if !defined(SYSTEM_H)
#define SYSTEM_H

#include "System definitions.h"

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#define SYSCLK  64000000
#elif defined(PN_GMR)
	#define SYSCLK  48000000
#endif

#define SYSTICK (SYSCLK/8) // 8MHz

#define UsToSysTick(us) (us*(SYSCLK*32/1000000))

#define IRQ_PRI_LOW      3
#define IRQ_PRI_MEDIUM   2
#define IRQ_PRI_HIGH     1
#define IRQ_PRI_REALTIME 0

extern volatile unsigned long SYS_SysTickMs;

void LaunchFirmware(void);
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4) 
	void TIM6_IRQHandler2(void);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	void TIM6_DAC_LPTIM1_IRQHandler2(void);
#elif defined(PN_GMR)		
	void TIM6_DAC_IRQHandler2(void);
#endif
void SystemInit(void);
void SYS_DelaySysTick(unsigned long Delay_SysTick);
void SYS_DelayUs(unsigned long Delay_us);
void SYS_DisableIRQs(void);
void SYS_EnableIRQs(void);
void SYS_Init(void);

#endif // !defined(SYSTEM_H)
