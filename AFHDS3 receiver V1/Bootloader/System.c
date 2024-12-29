#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#if defined(PN_FBR8)
#include "ADC.h"
#endif
#include "System definitions.h"

#include "System.h"

#include "LED.h"

volatile unsigned long SYS_SysTickMs;

// RDP=0xAA Level 0, read protection not active
// Reserved bits 8 to 12=0x01
// nRST_STOP=1 No reset generated when entering the Stop mode
// nRST_STDBY=1 No reset generate when entering the Standby mode
// Reserved bit 15=1
// IDWG_SW=1 Software independent watchdog
// IWDG_STOP=1 Independent watchdog counter is running in Stop mode
// IWDG_STDBY=1 Independent watchdog counter is running in Standby mode
// WWDG_SW=1 Software window watchdog
// Reserved bits 20 to 21=0x03
// RAM_PARITY_CHECK=1 SRAM parity check disable
// Reserved b&it 23=1
// nBOOT_SEL=1 BOOT0 signal is defined by nBOOT0 option bit
// nBOOT1=1
// nBOOT0=1
// Reserved bits 27 to 31=0x1B
#define FLASH_OPTION_BYTES (0xAA<<FLASH_OPTR_RDP_Pos) | (0x01<<8) | FLASH_OPTR_nRST_STOP_Msk | FLASH_OPTR_nRST_STDBY_Msk | (1<<15) | \
	FLASH_OPTR_IWDG_SW_Msk | FLASH_OPTR_IWDG_STOP_Msk | FLASH_OPTR_IWDG_STDBY_Msk | FLASH_OPTR_WWDG_SW_Msk | \
	(0x03<<20) | FLASH_OPTR_RAM_PARITY_CHECK_Msk | FLASH_OPTR_nBOOT_SEL_Msk | FLASH_OPTR_nBOOT1_Msk | FLASH_OPTR_nBOOT0_Msk | (0x1BUL<<27)

/*const unsigned long FlashOptionBytes[3] __attribute__((section(".ARM.__at_0x1FFF7800)))={
	FLASH_OPTION_BYTES,
	~FLASH_OPTION_BYTES,
	0xFFFFFFFF
};
*/

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4)|| defined(PN_FBR8) || defined(PN_FBR4)
	void TIM6_IRQHandler2(void)
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	void TIM6_DAC_LPTIM1_IRQHandler2(void)
#elif defined(PN_GMR)	
	void TIM6_DAC_IRQHandler2(void)
#endif
{
	if (TIM6->SR&TIM_SR_UIF)
	{
		TIM6->SR=~TIM_SR_UIF;
		SYS_SysTickMs++;
		LED_MsIRQHandler();
	}
	return;
}

void SystemInit(void)
{
	return;
}

void SYS_DelaySysTick(unsigned long Delay_SysTick)
{
	unsigned long DelaySysTick;
	unsigned long StartSysTick;

	StartSysTick=SysTick->VAL<<8;
	DelaySysTick=Delay_SysTick<<8;
	while (StartSysTick-(SysTick->VAL<<8)<DelaySysTick);
	return;
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

void SYS_DisableIRQs(void)
{
	__disable_irq();
	return;
}

void SYS_EnableIRQs(void)
{
	__enable_irq();
	return;
}

void SYS_Init(void)
{
	SYS_SysTickMs=0;

	// Enable the SysTick counter and set the maximum reload value
	SysTick->LOAD=0xFFFFFF;
	// SysTick SYSCLK/8=6M
	SysTick->CTRL|=1<<SysTick_CTRL_ENABLE_Pos;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
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
	
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4) 
	// Enable HSE clock
	RCC->CR|=RCC_CR_HSEON;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
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

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
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
	#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
		|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		// Enable the DBGMCU module clock	
		RCC->APBENR1|=RCC_APBENR1_DBGEN;
	#elif defined(PN_GMR)
		// Enable the DBGMCU module clock	
		RCC->APB2ENR|=RCC_APB2ENR_DBGMCUEN;
	#endif
#endif

	// Set TIM6 to be a 1ms periodic interrupt
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
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
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
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
	#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
		|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
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

	return;
}
