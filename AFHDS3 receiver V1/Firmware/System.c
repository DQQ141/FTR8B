#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "AFHDS3 queue low priority IRQ library.h"

#include "System receiver.h"

#include "ADC.h"
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include "i-Bus 1 device library.h"
	#include "i-Bus 2 device library.h"
#endif
#include "LED.h"
#include "New Port.h"
#include "SES AFHDS3.h"

volatile unsigned long SYS_SysTickMs;

volatile BOOL SYS_InLowPriorityCallback;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR)|| defined(PN_FBR8) || defined(PN_FBR4)
	void TIM6_IRQHandler(void)
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	void TIM6_DAC_LPTIM1_IRQHandler(void)
#endif
{
	if (TIM6->SR&TIM_SR_UIF)
	{
		TIM6->SR=~TIM_SR_UIF;
		SYS_SysTickMs++;
		SYS_InLowPriorityCallback=TRUE;
		SES_MsIRQHandler();
		SYS_InLowPriorityCallback=FALSE;
		ADC_MsIRQHandler();
		LED_MsIRQHandler();
		NPT_MsIRQHandler();
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
		|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		IB1D_MsIRQHandler();
		IB2D_MsIRQHandler();
#endif
	}
	return;
}

void WWDG_IRQHandler(void)
{
	SYS_InLowPriorityCallback=TRUE;
	SYS_LowPriorityIRQCallback();
	SYS_InLowPriorityCallback=FALSE;
	return;
}

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
void RTC_TAMP_IRQHandler(void)
#elif defined(PN_GMR)
void RTC_IRQHandler(void)
#endif
{
	SYS_MediumPriorityIRQCallback();
	return;
}

void SystemInit(void)
{
	return;
}

unsigned short SYS_CalcChecksum16(const void *pData,unsigned long DataLength)
{
	unsigned long ByteNb;
	unsigned short Checksum;

	Checksum=0;
	for (ByteNb=0;ByteNb<DataLength;ByteNb++)
	{
		Checksum+=*(const unsigned char *)pData;
		pData=(const unsigned char *)pData+1;
	}
	return Checksum^0xFFFF;
}

void SYS_DelayUs(unsigned long Delay_us)
{
	unsigned long DelaySysTick;
	unsigned long StartSysTick;

	StartSysTick=SysTick->VAL<<8;
	DelaySysTick=UsToSysTick(Delay_us);
	while (StartSysTick-(SysTick->VAL<<8)<DelaySysTick);
	return;
}

static unsigned char SYS_DisableIRQsCounter;

void SYS_DisableIRQs(void)
{
	if (SYS_DisableIRQsCounter==0)
		__disable_irq();
	SYS_DisableIRQsCounter++;
	return;
}

void SYS_EnableIRQs(void)
{
	SYS_DisableIRQsCounter--;
	if (SYS_DisableIRQsCounter==0)
		__enable_irq();
	return;
}

void SYS_Init(void)
{
	SYS_SysTickMs=0;
	SYS_DisableIRQsCounter=0;

	// Enable the SysTick counter and set the maximum reload value
	SysTick->LOAD=0xFFFFFF;
	// SysTick SYSCLK/8=6M
	SysTick->CTRL|=1<<SysTick_CTRL_ENABLE_Pos;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// Enable the SYSCFG clock
	RCC->APBENR2|=RCC_APBENR2_SYSCFGEN;
	// Enable the power interface clock
	RCC->APBENR1|=RCC_APBENR1_PWREN;

	// LATENCY=2 Two wait state
	// PRFTEN=1 Prefetch enabled
	// ICEN=1 CPU Instruction cache enabled
	// ICRST=0 CPU Instruction cache not reset
	// EMPTY=0 Main Flash memory area programmed
	FLASH->ACR=(FLASH->ACR & ~(FLASH_ACR_LATENCY_Msk | FLASH_ACR_PRFTEN_Msk | FLASH_ACR_ICEN_Msk | FLASH_ACR_ICRST_Msk | FLASH_ACR_PROGEMPTY_Msk)) |
		(FLASH_ACR_LATENCY_0*2) | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN;
	
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Enable HSE clock
	RCC->CR|=RCC_CR_HSEON;
#elif defined(PN_INR6_HS) || defined(PN_TMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	// Enable HSE clock
	RCC->CR|=RCC_CR_HSEON | RCC_CR_HSEBYP;
#endif
	// Wait for the HSE clock to be stable
	while (!(RCC->CR&RCC_CR_HSERDY));
	// Set PLL Clock (12(HSE)*4 MHz) to System Clock
	// Disable the PLL
	RCC->CR&=~RCC_CR_PLLON;
	// Wait for the PLL to be disabled
	while (RCC->CR&RCC_CR_PLLRDY);
	// PLLSRC=3 PLL input clock source is HSE
	// PLLM=0 Division factor of PLL input clock divider 1 (12MHz)
	// PLLN=16 PLL frequency multiplication factor 16 (192MHz)
	// PLLPEN=0 PLLPCLK clock output disabled
	// PLLP=2 PLL VCO division factor P for PLLPCLK clock output 3 (64MHz)
	// PLLREN=0 PLLURCLK clock output disabled
	// PLLR=2 PLL VCO division factor R for PLLPCLK clock output 3 (64MHz)
	RCC->PLLCFGR=(RCC_PLLCFGR_PLLSRC_0*3) | (RCC_PLLCFGR_PLLM_0*0) | (RCC_PLLCFGR_PLLN_0*16) | (RCC_PLLCFGR_PLLP_0*2) | (RCC_PLLCFGR_PLLR_0*2);
	// SW=0 HSISYS selected as system clock (default)
	// SWS read only
	// HPRE=0 AHB prescaler 1 HCLK=SYSCLK
	// PPRE=0 APB prescaler 1 PCLK=SYSCLK
	// MCOSEL=0 Microcontroller clock output disabled
	// MCOPRE=0 Microcontroller clock output prescaler 1
	RCC->CFGR=0;
	// Turn on the PLL
	RCC->CR|=RCC_CR_PLLON;
	// Enable the PLLR output
	RCC->PLLCFGR|=RCC_PLLCFGR_PLLREN;
	// Wait for the PLL to be ready (locked)
	while (!(RCC->CR&RCC_CR_PLLRDY));
	// Set the PLL output as system clock source
	// SW=2 PLLRCLK selected as system clock
	RCC->CFGR=(RCC->CFGR & ~RCC_CFGR_SW) | (RCC_CFGR_SW_0*2);
	// Wait for the PLL to have been selected as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_0*2);

	// Enable IO ports A, B, C, D and F clocks
	RCC->IOPENR|=RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN | RCC_IOPENR_GPIODEN | RCC_IOPENR_GPIOFEN;
	// Disable the pull-downs on PB15, PA8 and PD0, PD2
	SYSCFG->CFGR1=SYSCFG_CFGR1_UCPD1_STROBE|SYSCFG_CFGR1_UCPD2_STROBE;
	// Enable the DMA and DMAMUX clocks
	RCC->AHBENR|=RCC_AHBENR_DMA1EN;
	#if defined(DEBUG)
		// Enable the DBGMCU module clock	
		RCC->APBENR1|=RCC_APBENR1_DBGEN;
	#endif
#elif defined(PN_GMR)
	// Enable the SYSCFG clock
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;

	// LATENCY=1 <One wait state
	// PRFTBE=1 Prefetch enabled
	FLASH->ACR=FLASH_ACR_LATENCY | FLASH_ACR_PRFTBE;
	
	// Enable HSE clock
	RCC->CR|=RCC_CR_HSEON;
	// Wait for the HSE clock to be stable
	while (!(RCC->CR&RCC_CR_HSERDY));
	// Set PLL Clock (12(HSE)*4 MHz) to System Clock
	// Disable the PLL
	RCC->CR&=~RCC_CR_PLLON;
	// Wait for the PLL to be disabled
	while (RCC->CR&RCC_CR_PLLRDY);
	// Disable the PLL prescaler
	RCC->CFGR2=RCC_CFGR2_PREDIV_DIV1;
	// SW=0 HSI selected as system clock (default)
	// SWI=0 read only, set by hardware
	// HPRE=0 HAB clock = SYSCLK
	// PPRE=0 HCLK not divided
	// PLLSRC=1 PLL input source = HSE/PREDIV
	// PLLXTPRE=0 Disable the PLL prescaler
	// PLLMUL=2 PLL multiplication factor = 4
	// MCO=0 MCU clock output disabled
	// MCOPRE=0 ignored
	// PLLNODIV ignored
	RCC->CFGR=RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL4;
	// Turn on the PLL
	RCC->CR|=RCC_CR_PLLON;
	// Wait for the PLL to be ready (locked)
	while (!(RCC->CR&RCC_CR_PLLRDY));
	// Set the PLL output as system clock source
	RCC->CFGR=(RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
	// Wait for the PLL to have been selected as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_1);
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// Enable IO ports A, B, C, D and F clocks
	RCC->IOPENR|=RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN | RCC_IOPENR_GPIODEN | RCC_IOPENR_GPIOFEN;
	// Disable the pull-downs on PB15, PA8 and PD0, PD2
	SYSCFG->CFGR1=SYSCFG_CFGR1_UCPD1_STROBE|SYSCFG_CFGR1_UCPD2_STROBE;
	// Enable the DMA and DMAMUX clocks
	RCC->AHBENR|=RCC_AHBENR_DMA1EN;
#elif defined(PN_GMR)
	// Enable IO ports A, B, C, D and F and DMA clocks
	RCC->AHBENR|=RCC_AHBENR_DMAEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN | RCC_AHBENR_GPIOFEN;
#endif
#if defined(DEBUG)
	#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
		|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		// Enable the DBGMCU module clock	
		RCC->APBENR1|=RCC_APBENR1_DBGEN;
	#elif defined(PN_GMR)
		// Enable the DBGMCU module clock	
		RCC->APB2ENR|=RCC_APB2ENR_DBGMCUEN;
	#endif
#endif

	// Set TIM6 to be a 1ms periodic interrupt
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// Enable TIM6 clock
	RCC->APBENR1|=RCC_APBENR1_TIM6EN;
	// Reset TIM6
	RCC->APBRSTR1|=RCC_APBRSTR1_TIM6RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_TIM6RST;
#elif defined(PN_GMR)
	// Enable TIM6 clock
	RCC->APB1ENR|=RCC_APB1ENR_TIM6EN;
	// Reset TIM6
	RCC->APB1RSTR|=RCC_APB1RSTR_TIM6RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_TIM6RST;
#endif
	// CEN=0 counter disabled
	// UDIS=0 Update enabled
	// URS=1 Only counter overflow/underflow generates an update interrupt 
	// OPM=0 Counter is not stopped at update event
	// ARPE=0 Auto-reload preload disnabled
	// UIFREMAP=0 No remapping. UIF status bit is not copied to TIMx_CNT register bit 31
	TIM6->CR1=TIM_CR1_URS;
	// UIE=1 Update interrupt enable
	// UDE=0 Update DMA request disabled
	TIM6->DIER=TIM_DIER_UIE;
	// Prescaler=1MHz timer counter clock
	TIM6->PSC=SYSCLK/1000000-1;
	// Generate an update event to update the prescaler
	TIM6->EGR=TIM_EGR_UG;
	// Auto reload register set to 999 (counter overflows every 1ms)
	TIM6->ARR=1000-1;
	// Enable the timer counter
	TIM6->CR1|=TIM_CR1_CEN;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set the timer 6 IRQ to low priority and enable it
	NVIC_SetPriority(TIM6_IRQn,IRQ_PRI_LOW);
	NVIC_EnableIRQ(TIM6_IRQn);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	// Set the timer 6 IRQ to low priority and enable it
	NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn,IRQ_PRI_LOW);
	NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);
#endif
	// Trigger an event
	TIM6->EGR|=TIM_EGR_UG;

#if !defined(DEBUG)
	// Enable the watchdog
	// LSI RC oscillator frequency varies from 30 to 50KHz, set the watchdog on a 50KHz calculation for safety
	// The desired watchdog minimum trigger time is 10ms
	// Enable register access
	IWDG->KR=0x5555;
	// Set the prescaler to 4 (50KHz/4=12500Hz)
	IWDG->PR=0;
	// Set the reload register to 2500 (200ms timeout)
	IWDG->RLR=2500;
#endif

#if defined(DEBUG)
	#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
		|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		// Freeze everything when debug stop
		DBG->APBFZ1=DBG_APB_FZ1_DBG_TIM3_STOP | DBG_APB_FZ1_DBG_TIM6_STOP | DBG_APB_FZ1_DBG_TIM7_STOP | DBG_APB_FZ1_DBG_RTC_STOP |
			DBG_APB_FZ1_DBG_WWDG_STOP | DBG_APB_FZ1_DBG_IWDG_STOP | DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP;
		DBG->APBFZ2=DBG_APB_FZ2_DBG_TIM1_STOP | DBG_APB_FZ2_DBG_TIM14_STOP | DBG_APB_FZ2_DBG_TIM15_STOP |
			DBG_APB_FZ2_DBG_TIM16_STOP | DBG_APB_FZ2_DBG_TIM17_STOP;
	#elif defined(PN_GMR)
		// Freeze everything when debug stop
		DBGMCU->APB1FZ|=DBGMCU_APB1_FZ_DBG_TIM3_STOP | DBGMCU_APB1_FZ_DBG_TIM6_STOP | DBGMCU_APB1_FZ_DBG_TIM14_STOP | DBGMCU_APB1_FZ_DBG_RTC_STOP |
			DBGMCU_APB1_FZ_DBG_WWDG_STOP | DBGMCU_APB1_FZ_DBG_IWDG_STOP;
		DBGMCU->APB2FZ|=DBGMCU_APB2_FZ_DBG_TIM1_STOP | DBGMCU_APB2_FZ_DBG_TIM15_STOP | DBGMCU_APB2_FZ_DBG_TIM16_STOP | DBGMCU_APB2_FZ_DBG_TIM17_STOP;
	#endif
#endif

	SYS_InLowPriorityCallback=FALSE;
	// The watchdog IRQ is used for the low priority call back (Lowest priority)
	NVIC_SetPriority(WWDG_IRQn,IRQ_PRI_LOW);
	NVIC_EnableIRQ(WWDG_IRQn);
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// The RTC IRQ is used for the medium priority call back (Medium priority)
	NVIC_SetPriority(RTC_TAMP_IRQn,IRQ_PRI_MEDIUM);
	NVIC_EnableIRQ(RTC_TAMP_IRQn);
#elif defined(PN_GMR)
	// The RTC IRQ is used for the medium priority call back (medium priority)
	NVIC_SetPriority(RTC_IRQn,IRQ_PRI_MEDIUM);
	NVIC->ISER[0]=1UL<<RTC_IRQn;
#endif

	return;
}

__attribute((noreturn)) void SYS_ResetToBootloader(void)
{
	RAM_ResetSignature1=RESET_SIGNATURE_1;
	RAM_ResetSignature2=RESET_SIGNATURE_2;
	RAM_ResetMode=0;
	NVIC_SystemReset();
}

void SYS_ResetWatchdog(void)
{
#if !defined(DEBUG)
	IWDG->KR=0xAAAA;
#endif
	return;
}
void SYS_StartWatchdog(void)
{
#if !defined(DEBUG)
	IWDG->KR=0xCCCC;
#endif
	return;
}

void SYS_TriggerLowPriorityCallback(void)
{
	NVIC_SetPendingIRQ(WWDG_IRQn);
	return;
}

void SYS_TriggerMediumPriorityCallback(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	NVIC_SetPendingIRQ(RTC_TAMP_IRQn);
#elif defined(PN_GMR)
	NVIC_SetPendingIRQ(RTC_IRQn);
#endif
	return;
}
