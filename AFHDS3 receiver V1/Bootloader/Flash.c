#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System.h"
#include "System receiver.h"

#include "Flash.h"

static void FLH_ClearSR(void)
{
	unsigned long SR;
	
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	SR=FLASH->SR & (FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_PROGERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR |
		FLASH_SR_SIZERR | FLASH_SR_PGSERR | FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_OPTVERR);
#elif defined(PN_GMR)
	SR=FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_EOP | FLASH_SR_WRPERR);
#endif
	if (SR)
		FLASH->SR=SR;
	return;
}

BOOL FLH_ErasePage(unsigned long PageAddress)
{
	BOOL IsSuccess;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	unsigned long FlashCR;

	if (PageAddress>=FLASH_BASE_ADDRESS+FLASH_SIZE)
		return FALSE;
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	FLH_ClearSR();
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	// Page erase chosen.
	// PG=0 Flash memory programming disabled
	// PER=1 page erase enabled
	// MER1=0 Mass erase not enabled
	// PNB=Page number to erase
	// STRT=0 Do not start erasing yet
	// OPTSTRT=0 Do not start modification of option bytes
	// FSTPG=0 Fast programming disabled
	// EOPIE=1 End of operation interrupt enabled
	// ERRIE=0 Error interrupt disabled
	// OBL_LAUNCH=0 Not used
	// OPTLOCK=0 Do not lock option page
	// LOCK=0 Do not lock FLASH_CR
	FlashCR=FLASH_CR_PER | (((PageAddress-FLASH_BASE_ADDRESS)/FLASH_PAGE_SIZE)<<FLASH_CR_PNB_Pos) | FLASH_CR_EOPIE;
	FLASH->CR=FlashCR;
	// Start Erase
	FLASH->CR=FlashCR | FLASH_CR_STRT;
	// Wait for erase finished
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	IsSuccess=(FLASH->SR & FLASH_SR_EOP)?TRUE:FALSE;
	// Disable the PER  
	FLASH->CR=FlashCR & ~FLASH_CR_PER;
#elif defined(PN_GMR)
	while (FLASH->SR&FLASH_SR_BSY);
	FLH_ClearSR();
	while (FLASH->SR&FLASH_SR_BSY);
	// Page erase chosen.
	FLASH->CR|=FLASH_CR_PER;
	// Set erase address
	FLASH->AR=PageAddress;
	// Start Erase
	FLASH->CR|=FLASH_CR_STRT;
	// Wait for erase finished
	while (FLASH->SR&FLASH_SR_BSY);
	IsSuccess=(FLASH->SR & FLASH_SR_EOP)?TRUE:FALSE;
	// Disable the PER  
	FLASH->CR&=~FLASH_CR_PER;
#endif
	FLH_ClearSR();
	return IsSuccess;
}

void FLH_Lock(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	// Set the LOCK Bit to lock the FLASH control register and program memory access
	FLASH->CR|=FLASH_CR_LOCK;
#elif defined(PN_GMR)
	// Set the LOCK Bit to lock the FLASH control register and program memory access
	FLASH->CR|=FLASH_CR_LOCK;
#endif
	return;
}

BOOL FLH_Program(unsigned long ProgramAddress,const void *pData,unsigned long DataLength)
{
	DataLength=(DataLength+7)&~7UL;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	if (ProgramAddress+DataLength>=FLASH_BASE_ADDRESS+FLASH_SIZE)
#elif defined(PN_GMR)
	if (ProgramAddress+DataLength>=FLASH_BASE_ADDRESS+SYS_FLASH_SIZE)
#endif	
		return FALSE;
	while (DataLength)
	{
		if (FLH_ProgramDoubleWord(ProgramAddress,*(const unsigned long *)pData,*((const unsigned long *)pData+1)))
		{
			pData=(const void *)(((const unsigned long *)pData)+2);
			ProgramAddress+=8;
			DataLength-=8;
		}
		else
			return FALSE;
	}
	return TRUE;
}

BOOL FLH_ProgramDoubleWord(unsigned long ProgramAddress,unsigned long Word1,unsigned long Word2)
{
	BOOL IsSuccess;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	FLH_ClearSR();
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	// Page erase chosen.
	// PG=1 Flash memory programming enabled
	// PER=0 page erase disabled
	// MER1=0 Mass erase not enabled
	// PNB=0 Not used
	// STRT=0 Do not start erasing yet
	// OPTSTRT=0 Do not start modification of option bytes
	// FSTPG=0 Fast programming disabled
	// EOPIE=1 End of operation interrupt enabled
	// ERRIE=0 Error interrupt disabled
	// OBL_LAUNCH=0 Not used
	// OPTLOCK=0 Do not lock option page
	// LOCK=0 Do not lock FLASH_CR
	FLASH->CR=FLASH_CR_PG | FLASH_CR_EOPIE;
	*(volatile unsigned long *)ProgramAddress=Word1;
	*(volatile unsigned long *)(ProgramAddress+4)=Word2;
	while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
	IsSuccess=((FLASH->SR & FLASH_SR_EOP) &&
		*(volatile unsigned long *)ProgramAddress==Word1 &&
		*(volatile unsigned long *)(ProgramAddress+4)==Word2)?TRUE:FALSE;
	// Clear the EOP flag
	FLASH->SR=FLASH_SR_EOP;
	// Disable the PG  
	FLASH->CR=0;
#elif defined(PN_GMR)
	while (FLASH->SR&FLASH_SR_BSY);
	FLH_ClearSR();
	while (FLASH->SR&FLASH_SR_BSY);
	IsSuccess=FLH_ProgramHalfWord(ProgramAddress,Word1&0xFFFF) &&
		FLH_ProgramHalfWord(ProgramAddress+2,Word1>>16) &&
		FLH_ProgramHalfWord(ProgramAddress+4,Word2&0xFFFF) &&
		FLH_ProgramHalfWord(ProgramAddress+6,Word2>>16);
#endif
	FLH_ClearSR();
	return IsSuccess;
}

#if defined(PN_GMR)
// Flash is and stay unlocked
// Flash is not busy
BOOL FLH_ProgramHalfWord(unsigned long ProgramAddress,unsigned short HalfWord)
{
	BOOL IsSuccess;

	// Flash programming chosen.
	FLASH->CR|=FLASH_CR_PG;
	*(volatile unsigned short *)ProgramAddress=HalfWord;
	while (FLASH->SR&FLASH_SR_BSY);
	IsSuccess=((FLASH->SR & FLASH_SR_EOP) &&
		SYS_LoadUShort(ProgramAddress)==HalfWord)?TRUE:FALSE;
	// Disable the PG  
	FLASH->CR&=~FLASH_CR_PG;
	return IsSuccess;
}
#endif

void FLH_Unlock(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	if((FLASH->CR&FLASH_CR_LOCK)!=0)
	{
		while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_CFGBSY));
		// Unlocking the program memory access
		FLASH->KEYR=FLASH_KEY1;
		FLASH->KEYR=FLASH_KEY2;
	}
#elif defined(PN_GMR)
	while (FLASH->SR&FLASH_SR_BSY);
	if((FLASH->CR&FLASH_CR_LOCK)!=0)
	{
		// Unlocking the program memory access
		FLASH->KEYR=FLASH_KEY1;
		FLASH->KEYR=FLASH_KEY2;
	}
#endif
	return;
}
