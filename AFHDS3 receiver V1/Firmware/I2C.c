#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "I2C.h"

static BOOL I2C_Initialized;

void I2C_Init(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) ||  defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4) 
	// Set PB8 and PB9 as I2C1_SCL and I2C1_SDA
	GPIOB->AFR[1]=(GPIOB->AFR[1]&~((0xFUL<<((8-8)*4)) | (0xF<<((9-8)*4)))) | (6<<((8-8)*4)) | (6<<((9-8)*4));
	GPIOB->MODER=(GPIOB->MODER&~((3UL<<(8*2)) | (3<<(9*2)))) | (2<<(8*2)) | (2<<(9*2));
	// Enable the I2C1 module clock
	RCC->APBENR1|=RCC_APBENR1_I2C1EN;
	// Reset I2C1
	RCC->APBRSTR1|=RCC_APBRSTR1_I2C1RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_I2C1RST;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// Set PB6 and PB7 as I2C1_SCL and I2C1_SDA
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~((0xFUL<<(6*4)) | (0xFU<<(7*4)))) | (6<<(6*4)) | (6<<(7*4));
	GPIOB->MODER=(GPIOB->MODER&~((3UL<<(6*2)) | (3UL<<(7*2)))) | (2<<(6*2)) | (2<<(7*2));
	// Enable the I2C1 module clock
	RCC->APBENR1|=RCC_APBENR1_I2C1EN;
	// Reset I2C1
	RCC->APBRSTR1|=RCC_APBRSTR1_I2C1RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_I2C1RST;
#elif defined(PN_GMR)
	// Set PB6 and PB7 as I2C1_SCL and I2C1_SDA
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~((0xFUL<<(6*4)) | (0xFUL<<(7*4)))) | (1<<((6)*4)) | (1<<((7)*4));
	GPIOB->MODER=(GPIOB->MODER&~((3UL<<(6*2)) | (3<<(7*2)))) | (2<<(6*2)) | (2<<(7*2));
	// Set I2C clock to SYSCLK
	RCC->CFGR3|=RCC_CFGR3_I2C1SW;
	// Enable the I2C1 module clock
	RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;
	// Reset I2C1
	RCC->APB1RSTR|=RCC_APB1RSTR_I2C1RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_I2C1RST;
#endif
	// Disable the I2C peripheral
	I2C1->CR1&=~I2C_CR1_PE;
	// PE=0 Peripheral disabled
	// TXIE=0 TX interrupt disabled
	// RXIE=0 RX interrupt disabled
	// ADDRIE=0 Address match interrupt disabled
	// NACKIE=0 Not acknowledge received interrupt disabled
	// STOPIE=0 STOP detection interrupt disabled
	// TCIE=0 Transfer complete interrupt disabled
	// ERRIE=0 Error interrupt disabled
	// DNF=4 Digital filter set to 4*tI2CCLK
	// ANFOFF=1 Analog noise filter OFF
	// TXDMAEN=0 TX DMA disabled
	// RXDMAEN=0 RX DMA disabled
	// SBC=0 Slave byte control disabled
	// NOSTRETCH=0 Clock stretching enabled
	// WUPEN=0 Wakeup from stop disabled
	// GCEN=0 General call disabled
	// SMBHEN=0 SMBus host address disabled
	// SMBDEN=0 SMBus device default address disabled
	// ALERTEN=0 SMBus alert disabled
	// PECEN=0 PEC calculation disabled
	I2C1->CR1=(4<<I2C_CR1_DNF_Pos) | I2C_CR1_ANFOFF;
	// SCLL=I2C_SCK_LOW_TSCK-1
	// SCLH=I2C_SCK_HIGH_TSCK-1
	// SDADEL=I2C_SDA_DEL_TSCK
	// SCLDEL=I2C_SCL_DEL_TSCK-1
	// PRESC=0 Prescaler disabled
	I2C1->TIMINGR=((I2C_SCK_LOW_TSCK-1)<<I2C_TIMINGR_SCLL_Pos) | ((I2C_SCK_HIGH_TSCK-1)<<I2C_TIMINGR_SCLH_Pos) | 
		(I2C_SDA_DEL_TSCK<<I2C_TIMINGR_SDADEL_Pos) | ((I2C_SCL_DEL_TSCK-1)<<I2C_TIMINGR_SCLDEL_Pos) | (0<<I2C_TIMINGR_PRESC_Pos);
	// Enable the I2C peripheral
	I2C1->CR1|=I2C_CR1_PE;
	I2C_Initialized=TRUE;
	return;
}

BOOL I2C_Read(unsigned char Address,void *pData,unsigned long DataLength,BOOL SendStop)
{
	return I2C_Transfer(FALSE,Address,pData,DataLength,SendStop);
}

BOOL I2C_Transfer(BOOL IsWrite,unsigned char Address,void *pData,unsigned long DataLength,BOOL SendStop)
{
	BOOL FirstBlock;
	unsigned long RemainingDataLength;
	
	FirstBlock=TRUE;
	RemainingDataLength=DataLength;
	do
	{
		unsigned long BlockLength;
		unsigned long BlockLength2;
		
		BlockLength=RemainingDataLength;
		if (BlockLength>0xFF)
			BlockLength=0xFF;
		RemainingDataLength-=BlockLength;
		{
			unsigned long CR2;

			// SADDR=0 Not used
			// SADD=Address slave address
			// RD_WRN=0 Read or write transfer
			// ADD10=0 7-bit addressing mode
			// HEAD10R=0 Not used
			// START=0 Do not generate a start yet
			// STOP=0 Do not gerenate a stop
			// NACK=0 Not used
			// NBYTES=DataLength Data length
			// RELOAD=0 DataLength<256
			// AUTOEND=SendStop
			// PECBYTE=0 No PEC transfer
			CR2=((unsigned long)Address<<(I2C_CR2_SADD_Pos+1)) | ((IsWrite)?0UL:I2C_CR2_RD_WRN) | (BlockLength<<I2C_CR2_NBYTES_Pos);
			if (DataLength<=0xFF ||
				RemainingDataLength==0)
				CR2|=((SendStop)?I2C_CR2_AUTOEND:0);
			else
				CR2|=I2C_CR2_RELOAD;
			if (FirstBlock)
				CR2|=I2C_CR2_START;
			I2C1->CR2=CR2;
			// Wait for the end of the start
			if (FirstBlock)
			{
				FirstBlock=FALSE;
				while (I2C1->CR2 & I2C_CR2_START);
			}
		}
		// NACK when sending the address?
		if (I2C1->ISR&I2C_ISR_NACKF)
		{
			while (!(I2C1->ISR & I2C_ISR_STOPF));
			I2C1->ICR=I2C_ICR_NACKCF | I2C_ICR_STOPCF;
			while (I2C1->ISR & I2C_ISR_STOPF);
			return FALSE;
		}
		BlockLength2=BlockLength;
		while (BlockLength2)
		{
			unsigned long ISR;

			do
			{
				ISR=I2C1->ISR;
				if (ISR&I2C_ISR_NACKF)
				{
					while (!(I2C1->ISR & I2C_ISR_STOPF));
					I2C1->ICR=I2C_ICR_NACKCF | I2C_ICR_STOPCF;
					while (I2C1->ISR & I2C_ISR_STOPF);
					return FALSE;
				}
			} while (( IsWrite && !(ISR&(I2C_ISR_TXIS | I2C_ISR_TC | I2C_ISR_TCR))) ||
					 (!IsWrite && !(I2C1->ISR & I2C_ISR_RXNE)));
			if (IsWrite)
				I2C1->TXDR=*(unsigned char *)pData;
			else
				*(unsigned char *)pData=(unsigned char)I2C1->RXDR;
			pData=(unsigned char *)pData+1;
			BlockLength2--;
		}
		while (!(I2C1->ISR & (I2C_ISR_TC | I2C_ISR_TCR | I2C_ISR_STOPF)));
	} while (RemainingDataLength);
	if (SendStop)
	{
		while (!(I2C1->ISR & I2C_ISR_STOPF));
		I2C1->ICR=I2C_ICR_STOPCF;
		while (I2C1->ISR & I2C_ISR_STOPF);
	}
	return TRUE;
}

BOOL I2C_Write(unsigned char Address,const void *pData,unsigned long DataLength,BOOL SendStop)
{
#pragma clang diagnostic ignored "-Wcast-qual"
	return I2C_Transfer(TRUE,Address,(void *)pData,DataLength,SendStop);
#pragma clang diagnostic warning "-Wcast-qual"
}
