#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "AFHDS3 CRC library.h"
#include "LED.h"
#include "SPI SX1280.h"

#include "Bootloader library.h"
#include "Bootloader PHY.h"
#include "System receiver.h"
#include "Poweronch.h"

#if !defined(DEBUG)
#include "Encryption.h"
#endif

#include "Main.h"
#if defined(PN_FBR8)
#include "ADC.h"
static unsigned char ADC_SampleNb;
static unsigned long ADC_ChannelsSumValue;
static unsigned long ADCData=0;
#endif

static sBLL_Init BLL_Init;


#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4)
void EXTI4_15_IRQHandler2(void)
{
	if (EXTI->FPR1&(1<<14))
	{
		EXTI->FPR1=1<<14;
		SPI_SX1280_BusyIRQHandler();
		return;
	}
	if (EXTI->RPR1&(1<<13))
	{
		EXTI->RPR1=1<<13;
		PHY_SX1280_DIO1IRQHandler();
		return;
	}
	return;
}
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
void EXTI4_15_IRQHandler2(void)
{
	if (EXTI->FPR1&(1<<12))
	{
		EXTI->FPR1=1<<12;
		SPI_SX1280_BusyIRQHandler();
		return;
	}
	if (EXTI->RPR1&(1<<8))
	{
		EXTI->RPR1=1<<8;
		PHY_SX1280_DIO1IRQHandler();
		return;
	}
	return;
}
#elif defined(PN_GMR)
void EXTI0_1_IRQHandler2(void)
{
	if (EXTI->PR&(1<<0))
	{
		EXTI->PR=1<<0;
		SPI_SX1280_BusyIRQHandler();
		return;
	}
	return;
}

void EXTI2_3_IRQHandler2(void)
{
	if (EXTI->PR&(1<<2))
	{
		EXTI->PR=1<<2;
		PHY_SX1280_DIO1IRQHandler();
		return;
	}
	return;
}
#endif

#pragma clang diagnostic ignored "-Wunused-parameter"
void PHY_SetAntenna(unsigned char AntennaNb)
#pragma clang diagnostic warning "-Wunused-parameter"
{
#if defined(PN_FTR8B) || defined(PN_FTR12B) || defined(PN_FBR12)
	if (AntennaNb==0)
	{
		// Set PC13=0 (Antenna 1 switch, active low)
		GPIOC->BSRR=1UL<<(13+11);
		// Set PC15=1 (Antenna 2 switch, active low)
		GPIOC->BSRR=1<<15;
	}
	else
	{
		// Set PC15=0 (Antenna 2 switch, active low)
		GPIOC->BSRR=1UL<<(16+15);
		// Set PC13=1 (Antenna 1 switch, active low)
		GPIOC->BSRR=1<<13;
	}
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	if (AntennaNb==0)
	{
		// Set PB3=0 (Antenna 1 switch, active low)
		GPIOB->BSRR=1UL<<(16+3);
		// Set PB4=1 (Antenna 2 switch, active low)
		GPIOB->BSRR=1<<4;
	}
	else
	{
		// Set PB4=0 (Antenna 2 switch, active low)
		GPIOB->BSRR=1UL<<(16+4);
		// Set PB3=1 (Antenna 1 switch, active low)
		GPIOB->BSRR=1<<3;
	}
#elif defined(PN_TR8B)
	if (AntennaNb==0)
	{
		// Set PA15=0 (Antenna 1 switch, active low)
		GPIOA->BSRR=1UL<<(16+15);
		// Set PB2=1 (Antenna 2 switch, active low)
		GPIOB->BSRR=1<<2;
	}
	else
	{
		// Set PB2=0 (Antenna 2 switch, active low)
		GPIOB->BSRR=1UL<<(16+2);
		// Set PA15=1 (Antenna 1 switch, active low)
		GPIOA->BSRR=1<<15;
	}
#elif defined(PN_FGR8B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR)
#endif
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void PHY_SetAttenuator(BOOL AttenuatorOn)
#pragma clang diagnostic warning "-Wunused-parameter"
{
#if defined(PN_NONE)
	if (AttenuatorOn)
	{
		// Set PC6=0 (Attenuator high)
		GPIOC->BSRR=1UL<<(16+6);
		// Set PC7=1 (Attenuator low)
		GPIOC->BSRR=1<<7;
	}
	else
	{
		// Set PC7=0 (Attenuator low)
		GPIOC->BSRR=1<<(16+7);
		// Set PC6=1 (Attenuator high)
		GPIOC->BSRR=1UL<<6;
	}
#elif defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
#endif
	return;
}
#pragma clang diagnostic ignored "-Wunused-parameter"
void PHY_SetPAMode(ePHY_PAMode PAMode)
#pragma clang diagnostic warning "-Wunused-parameter"
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	switch (PAMode)
	{
	case PHY_PAM_IDLE:
		// Set PC14=0 (PA TX switch, active high)
		GPIOC->BSRR=1<<(16+14);
		// Set PA10=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+10);
		break;
	case PHY_PAM_TX:
		// Set PA10=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+10);
		// Set PC14=1 (PA TX switch, active high)
		GPIOC->BSRR=1<<14;
		break;
	case PHY_PAM_RX:
		// Set PC14=0 (PA TX switch, active high)
		GPIOC->BSRR=1<<(16+14);
		// Set PA10=1 (PA RX switch, active high)
		GPIOA->BSRR=1<<10;
		break;
	}
#elif defined(PN_FGR4D)
	switch (PAMode)
	{
	case PHY_PAM_IDLE:
		// Set PB5=0 (PA TX switch, active high)
		GPIOB->BSRR=1<<(16+5);
		// Set PA11=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+11);
		break;
	case PHY_PAM_TX:
		// Set PA11=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+11);
		// Set PB5=1 (PA TX switch, active high)
		GPIOB->BSRR=1<<5;
		break;
	case PHY_PAM_RX:
		// Set PB5=0 (PA TX switch, active high)
		GPIOB->BSRR=1<<(16+5);
		// Set PA11=1 (PA RX switch, active high)
		GPIOA->BSRR=1<<11;
		break;
	}
#elif defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)
#endif
	return;
}

#if defined(PHY_SX1280)
BOOL PHY_SX1280_IsBusy(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	return (GPIOB->IDR&(1<<14))>>14;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	return (GPIOA->IDR&(1<<12))>>12;
#elif defined(PN_GMR)
	return (GPIOB->IDR&(1<<0))>>0;
#endif
}

void PHY_SX1280_SetResetPin(BOOL Level)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	GPIOA->BSRR=(Level)?1<<9:1<<(16+9);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	GPIOB->BSRR=(Level)?1<<9:1<<(16+9);
#elif defined(PN_GMR)
	GPIOA->BSRR=(Level)?1<<0:1<<(16+0);
#endif
	return;
}
#endif

__attribute__((noreturn)) void APP_SystemReboot(void)
{
	NVIC_SystemReset();
}

int main(void)
{
	BOOL IsUpdateMode;
	
	RAM_IsFirmwareRunning=0;		
	
	if (RAM_ResetSignature1==FIRMWARE_SIGNATURE_1 &&
		RAM_ResetSignature2==FIRMWARE_SIGNATURE_2)
	{
	#if !defined(DEBUG)
		if(EncryptionFlag==ENCRYPTION_SIGNATURE)
		{
			Encrypt_WriteCode();
			FLH_Unlock();
			FLH_ErasePage(ENCRYPTION_BASE_ADDRESS);	
			FLH_Lock();
		}
	#endif		
		RAM_ResetSignature1=0;
		RAM_ResetSignature2=0;
		RAM_IsFirmwareRunning=1;
		LaunchFirmware();
	}
	SYS_Init();
	PHY_PowerOnPortCHxSet();
	LED_Init();
#if defined(PN_FBR8)
	ADC_Init();
#endif
	LED_SetPattern(&LED_Bind);

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB)
	// Set PA15 as input (Bind port)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (0<<(15*2));
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE;
	if(!(GPIOA->IDR&(1<<15)))
	{
		unsigned long ButtonPressingStartMs;
		
		ButtonPressingStartMs=SYS_SysTickMs;
		while (!(GPIOA->IDR&(1<<15)) && SYS_SysTickMs-ButtonPressingStartMs<10000);
		if (SYS_SysTickMs-ButtonPressingStartMs>=10000)
			IsUpdateMode=TRUE;
		else if (SYS_SysTickMs-ButtonPressingStartMs>=100)
			RAM_IsBinding=TRUE;
	}
#elif defined(PN_FBR12)
	// Set PA7 as input (Bind Line)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(7*2))) | (0<<(7*2));
	// Set PA7 as Pull-UP 
	GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(7*2))) | (1<<(7*2));
	 // Set PB1 as output
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(1*2))) | (1<<(1*2));
	// Set PB1=1 
	GPIOB->BSRR=1<<1;
	SYS_DelaySysTick(100);
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE; 
	if(GPIOA->IDR&(1UL<<7))
	{
		// Set PB1=0 
		GPIOB->BSRR=1UL<<(16+1);
		// Wait for 100us
		SYS_DelaySysTick(100);
		if (!(GPIOA->IDR&(1<<7)))
			IsUpdateMode=TRUE;
	}
	else if(!(GPIOA->IDR&(1<<7)))
	{
		// Wait for 100us
		SYS_DelaySysTick(100);
		if (!(GPIOA->IDR&(1<<7)))
			RAM_IsBinding=TRUE;
	}  
#elif defined(PN_FBR8)
	
	// Set PA11 as input (Update Line)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (0<<(11*2));
	// Set PA11 as Pull-UP 
	GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(11*2))) | (1<<(11*2));
	 // Set PB3 as output
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(3*2))) | (1<<(3*2));
	
	// Set PB6 PB7 as input (Bind Line)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(6*2))) | (1<<(6*2));
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(7*2))) | (1<<(7*2));

	
	// Set PB3=0 
	GPIOB->BSRR=1UL<<(16+3);
	// Set PB6 PB7=1 
	GPIOB->BSRR=1<<6;
	GPIOB->BSRR=1<<7;
	// Wait for 100us
	SYS_DelaySysTick(100);	
	for(;;)
	{// Sum the last conversion result
		if (ADC_SampleNb==0)
			ADC_ChannelsSumValue=ADC1->DR;
		else
			ADC_ChannelsSumValue+=ADC1->DR;
		ADC_SampleNb++;
		if (ADC_SampleNb>=ADC_NB_SAMPLES)
		{
			ADC_SampleNb=0;
			ADCData=ADC_ChannelsSumValue/ADC_NB_SAMPLES;
			break;
		}	
		ADC1->CHSELR=1<<7;
		// Start the next conversion
		ADC1->CR|=ADC_CR_ADSTART;
		// Wait for 1000us
		SYS_DelaySysTick(1000);	
	}
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE; 
	if(!(GPIOA->IDR&(1<<11)))
	{
		// Set PB3=1  
		GPIOB->BSRR=1<<3;
		// Wait for 100us
		SYS_DelaySysTick(100);
		if (GPIOA->IDR&(1UL<<11))
			IsUpdateMode=TRUE;
	}
	else if(ADCData>=210)//(!(GPIOA->IDR&(1<<7)))//(!(GPIOA->IDR&(1<<7)))
	{
		GPIOB->BSRR=1UL<<(16+6);
		GPIOB->BSRR=1UL<<(16+7);
		// Wait for 100us
		SYS_DelaySysTick(100);
		for(;;)
		{// Sum the last conversion result
			if (ADC_SampleNb==0)
				ADC_ChannelsSumValue=ADC1->DR;
			else
				ADC_ChannelsSumValue+=ADC1->DR;
			ADC_SampleNb++;
			if (ADC_SampleNb>=ADC_NB_SAMPLES)
			{
				ADC_SampleNb=0;
				ADCData=ADC_ChannelsSumValue/ADC_NB_SAMPLES;
				break;
			}	
			ADC1->CHSELR=1<<7;
			// Start the next conversion
			ADC1->CR|=ADC_CR_ADSTART;
			// Wait for 1000us
			SYS_DelaySysTick(1000);	
		}	
		ADCData=ADC1->DR;
		if (ADCData<=50)//(!(GPIOA->IDR&(1<<7)))
			RAM_IsBinding=TRUE;
	}      
#elif defined(PN_FBR4)
	// Set PA7 as input (Bind Line)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(7*2))) | (0<<(7*2));
	// Set PA7 as Pull-UP 
	GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(7*2))) | (1<<(7*2));
	// Set PA2 as output
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
    // Set PA3 as output
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(3*2))) | (1<<(3*2));
	// Set PA2=1 
	GPIOA->BSRR=1<<2;
    // Set PA3=1 
	GPIOA->BSRR=1<<3;
	SYS_DelaySysTick(100);
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE; 
	if(GPIOA->IDR&(1UL<<7))
	{
		// Set PA2=0 
		GPIOA->BSRR=1UL<<(16+2);
        // Set PA3=0 
		GPIOA->BSRR=1UL<<(16+3);
		// Wait for 100us
		SYS_DelaySysTick(100);
		if (!(GPIOA->IDR&(1<<7)))
			IsUpdateMode=TRUE;
	}
	else if(!(GPIOA->IDR&(1<<7)))
	{
		// Wait for 100us
		SYS_DelaySysTick(100);
		if (!(GPIOA->IDR&(1<<7)))
			RAM_IsBinding=TRUE;
	}      
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	// Set PA8 as input (Bind port)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(8*2))) | (0<<(8*2));
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE;
	if(!(GPIOA->IDR&(1<<8)))
	{
		unsigned long ButtonPressingStartMs;
		
		ButtonPressingStartMs=SYS_SysTickMs;
		while (!(GPIOA->IDR&(1<<8)) && SYS_SysTickMs-ButtonPressingStartMs<10000);
		if (SYS_SysTickMs-ButtonPressingStartMs>=10000)
			IsUpdateMode=TRUE;
		else if (SYS_SysTickMs-ButtonPressingStartMs>=100)
			RAM_IsBinding=TRUE;
	}
#elif defined(PN_TR8B)
	// Set PB3 as input (CH3 port)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(3*2))) | (0<<(3*2));
	GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(3*2))) | (1<<(3*2));
	// Set PA11 as output (CH4 connected to the bind port with a bind cable)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
	// Output 1 on PA11
	GPIOA->BSRR=1UL<<( 0+11);
	// Wait for 100us
	SYS_DelaySysTick(100);	
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE;
	if(GPIOB->IDR&(1<<3))
	{
		// Output 0 on PA11
		GPIOA->BSRR=1UL<<(16+11);
		// Wait for 100us
		SYS_DelaySysTick(100);
		if (!(GPIOB->IDR&(1<<3)))
			IsUpdateMode=TRUE;
	}
#elif defined(PN_GMR)
	// Set PA12 as input (Bind port)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(12*2))) | (0<<(12*2));
	RAM_IsBinding=FALSE;
	IsUpdateMode=FALSE;
	if(!(GPIOA->IDR&(1<<12)))
	{
		unsigned long ButtonPressingStartMs;
		
		ButtonPressingStartMs=SYS_SysTickMs;
		while (!(GPIOA->IDR&(1<<12)) && SYS_SysTickMs-ButtonPressingStartMs<10000);
		if (SYS_SysTickMs-ButtonPressingStartMs>=10000)
			IsUpdateMode=TRUE;
		else if (SYS_SysTickMs-ButtonPressingStartMs>=100)
			RAM_IsBinding=TRUE;
	}
#endif

	// Check if a valid firmware is present
	BLL_Init.ProductNumber=BOOT_ProductNumber;
	BLL_Init.FlashConfigBaseAddress=FLASH_BASE_ADDRESS+BOOTLOADER_SIZE-FLASH_PAGE_SIZE;
	BLL_Init.FlashFirmwareBaseAddress=FLASH_BASE_ADDRESS+BOOTLOADER_SIZE;
	if (
#if defined(DEBUG)
		(FIRMWARE_Signature1==FIRMWARE_DEBUG_SIGNATURE_1 &&
		 FIRMWARE_Signature2==FIRMWARE_DEBUG_SIGNATURE_2) ||
#endif
		(FIRMWARE_Signature1==FIRMWARE_SIGNATURE_1 &&
		 FIRMWARE_Signature2==FIRMWARE_SIGNATURE_2 &&
		 FIRMWARE_Length<MAX_FIRMWARE_SIZE &&
		 SYS_CalcCRC32((void *)(FLASH_BASE_ADDRESS+BOOTLOADER_SIZE),FIRMWARE_Length)==
			SYS_LoadULong(FLASH_BASE_ADDRESS+BOOTLOADER_SIZE+FIRMWARE_Length)))
	{
		// Launch the firmware if not in update mode and the reset signature is not valid
		if (!IsUpdateMode &&
			(RAM_ResetSignature1!=RESET_SIGNATURE_1 ||
			 RAM_ResetSignature2!=RESET_SIGNATURE_2))
		{
			RAM_ResetSignature1=FIRMWARE_SIGNATURE_1;
			RAM_ResetSignature2=FIRMWARE_SIGNATURE_2;
			NVIC_SystemReset();
		}
		// If not, stay in the bootloader and wait for a valid command
#if defined(DEBUG)
		if (FIRMWARE_Signature1==FIRMWARE_DEBUG_SIGNATURE_1 &&
			FIRMWARE_Signature2==FIRMWARE_DEBUG_SIGNATURE_2)
			BLL_Init.IsValidFirmwarePresent=FALSE;
#endif
		else
		{
			BLL_Init.IsValidFirmwarePresent=TRUE;
			BLL_Init.FirmwareVersion=FIRMWARE_Version;
			BLL_Init.FirmwareLength=FIRMWARE_Length;
			BLL_Init.FirmwareCRC32=SYS_LoadULong(FIRMWARE_BASE_ADDRESS+FIRMWARE_Length);
		}
	}
	else
		BLL_Init.IsValidFirmwarePresent=FALSE;
	RAM_ResetSignature1=0;
	RAM_ResetSignature2=0;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set PA9 as output (SX1280 RESET)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (1<<(9*2));
	// Set PB14 as input (SX1280 BUSY)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(14*2))) | (0<<(14*2));
	// Set PB13 as input (SX1280 DIO1)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(13*2))) | (0<<(13*2));
	// Set an interrupt on PB13 rising (DIO1)
	// Enable external interrupt 13 on port B
	EXTI->EXTICR[13/4]=(EXTI->EXTICR[13/4]&0xFFFF00FF) | 0x00000100;
	// Unmask interrupt request from line 13
	EXTI->IMR1|=1<<13;
	// Enable rising trigger from input line 13
	EXTI->RTSR1|=1<<13;
	// Disable falling trigger from input line 13
	EXTI->FTSR1&=~(1UL<<13);
	// Set an interrupt on PB14 falling (Busy)
	// Enable external interrupt 14 on port B
	EXTI->EXTICR[14/4]=(EXTI->EXTICR[14/4]&0xFF00FFFF) | 0x00010000;
	// Unmask interrupt request from line 14
	EXTI->IMR1|=1<<14;
	// Disable rising trigger from input line 14
	EXTI->RTSR1&=~(1UL<<14);
	// Enable falling trigger from input line 14
	EXTI->FTSR1|=1<<14;
	// Set the EXTI2_3 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI4_15_IRQn,IRQ_PRI_HIGH);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	// Set PB9 as output (SX1280 RESET)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(9*2))) | (1<<(9*2));
	// Set PA12 as input (SX1280 BUSY)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(12*2))) | (0<<(12*2));
	// Set PB8 as input (SX1280 DIO1)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(8*2))) | (0<<(8*2));
	// Set an interrupt on PB8 rising (DIO1)
	// Enable external interrupt 8 on port B
	EXTI->EXTICR[8/4]=(EXTI->EXTICR[8/4]&0xFFFFFF00) | 0x00000001;
	// Unmask interrupt request from line 8
	EXTI->IMR1|=1<<8;
	// Enable rising trigger from input line 8
	EXTI->RTSR1|=1<<8;
	// Disable falling trigger from input line 8
	EXTI->FTSR1&=~(1UL<<8);
	// Set an interrupt on PA12 falling (Busy)
	// Enable external interrupt 12 on port A
	EXTI->EXTICR[12/4]=(EXTI->EXTICR[12/4]&0xFFFFFF00) | 0x00000000;
	// Unmask interrupt request from line 12
	EXTI->IMR1|=1<<12;
	// Disable rising trigger from input line 12
	EXTI->RTSR1&=~(1UL<<12);
	// Enable falling trigger from input line 12
	EXTI->FTSR1|=1<<12;
	// Set the EXTI2_3 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI4_15_IRQn,IRQ_PRI_HIGH);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
#elif defined(PN_GMR)
	// Set PA0 as output (SX1280 RESET)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (1<<(0*2));

	// Set PB0 as input (SX1280 BUSY)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (0<<(0*2));
	// Enable external interrupt 0 on port B (DIO1)
	SYSCFG->EXTICR[0/4]=(SYSCFG->EXTICR[0/4]&0xFFF0) | 0x0001;
	// Set an interrupt on PB0 falling (Busy)
	// Unmask interrupt request from line 0
	EXTI->IMR|=1<<0;
	// Enable rising trigger from input line 0
	EXTI->RTSR&=~(1UL<<0);
	// Disable falling trigger from input line 0
	EXTI->FTSR|=(1UL<<0);
	// Enable external interrupt 0 on port B (Busy)	
	// Set the EXTI0_1 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI0_1_IRQn,IRQ_PRI_HIGH);
	NVIC->ISER[0]=1UL<<EXTI0_1_IRQn;
	
	// Set PB2 as input (SX1280 DIO1)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (0<<(2*2));
	SYSCFG->EXTICR[2/4]=(SYSCFG->EXTICR[2/4]&0xF0FF) | 0x0100;
	// Set an interrupt on PB2 rising (DIO1)
	// Unmask interrupt request from line 2
	EXTI->IMR|=1<<2;
	// Disable rising trigger from input line 2
	EXTI->RTSR|=(1UL<<2);
	// Enable falling trigger from input line 2
	EXTI->FTSR&=~(1UL<<2);	
	// Set the EXTI2_3 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI2_3_IRQn,IRQ_PRI_HIGH);
	NVIC->ISER[0]=1UL<<EXTI2_3_IRQn;
#endif

#if defined(PN_FTR8B) || defined(PN_FTR12B) || defined(PN_FBR12)
	// Set PC13 as output (Antenna 1 switch, active low)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(13*2))) | (1<<(13*2));
	// Set PC15 as output (Antenna 2 switch, active low)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(15*2))) | (1<<(15*2));
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	// Set PB3 as output (Antenna 1 switch, active low)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(3*2))) | (1<<(3*2));
	// Set PB4 as output (Antenna 2 switch, active low)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(4*2))) | (1<<(4*2));
#elif defined(PN_TR8B)
	// Set PA15 as output (Antenna 1 switch, active low)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (1<<(15*2));
	// Set PB2 as output (Antenna 2 switch, active low)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (1<<(2*2));
#elif defined(PN_FGR8B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR)
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set PC14 as output (PA TX switch, active high)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(14*2))) | (1<<(14*2));
	// Set PA10 as output (PA RX switch, active high)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (1<<(10*2));
#elif defined(PN_FGR4D)
	// Set PB5 as output (PA TX switch, active high)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(5*2))) | (1<<(5*2));
	// Set PA11 as output (PA RX switch, active high)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
#elif defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4)
#elif defined(PN_NONE)
	// Set PC7 as output (Attenuator low)
	GPIOC->MODER=(GPIOC->MODER&~(3<<(7*2))) | (1<<(7*2));
	// Set PC6 as output (Attenuator high)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(6*2))) | (1<<(6*2));
#endif

	BootloaderLibrary(&BLL_Init);
	
}
