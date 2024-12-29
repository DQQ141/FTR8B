#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
#include <stm32g0xx.h>
#include "Flash.h"
#include "System.h"

#define FLASH_KEY1                        0x45670123U                      
#define FLASH_KEY2                        0xCDEF89ABU    

#define FLASH_OPTKEY1                     0x08192A3BU                      
#define FLASH_OPTKEY2                     0x4C5D6E7FU 

#define FLASH_ER_PRG_TIMEOUT    ((uint32_t)0x000B0000)

static void FLASH_Unlock(void)
{
	if(FLASH->CR & FLASH_CR_LOCK)
	{
		/* Unlocking the program memory access */
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
}

static void FLASH_Lock(void)
{
	/* Set the LOCK Bit to lock the FLASH control register and program memory access */
	FLASH->CR |= FLASH_CR_LOCK;
}

static void FLASH_OB_Unlock(void)
{
	//If option byte write is disabled,enable it
	if(FLASH->CR & FLASH_CR_OPTLOCK)
	{ 
		// Unlocking the option bytes block access
		FLASH->OPTKEYR = FLASH_OPTKEY1;
		FLASH->OPTKEYR = FLASH_OPTKEY2;
	}
}

static void FLASH_OB_Lock(void)
{
	//Set the OPTWREN Bit to lock the option bytes block access 
	FLASH->CR |= FLASH_CR_OPTLOCK;
}

static void FLASH_OB_Launch(void)
{
	//Set the OBL_Launch bit to launch the option byte loading 
	FLASH->CR |= FLASH_CR_OBL_LAUNCH;
}

static FLASH_Status FLASH_GetStatus(void)
{
	FLASH_Status FLASHstatus = FLASH_COMPLETE;
	
	if((FLASH->SR & FLASH_SR_BSY1) == FLASH_SR_BSY1) 
	{
		FLASHstatus = FLASH_BUSY;
	}
	else 
	{  
		if((FLASH->SR & (uint32_t)FLASH_SR_WRPERR)!= (uint32_t)0x00)
		{ 
			FLASHstatus = FLASH_ERROR_WRP;
		}
		else 
		{
			if((FLASH->SR & (uint32_t)(FLASH_SR_PGSERR)) != (uint32_t)0x00)
			{
				FLASHstatus = FLASH_ERROR_PROGRAM; 
			}
			else
			{
				FLASHstatus = FLASH_COMPLETE;
			}
		}
	}
	/* Return the FLASH Status */
	return FLASHstatus;
}

static FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
	FLASH_Status status = FLASH_COMPLETE;
	 
	/* Check for the FLASH Status */
	status = FLASH_GetStatus();
	
	/* Wait for a FLASH operation to complete or a TIMEOUT to occur */
	while((status == FLASH_BUSY) && (Timeout != 0x00))
	{
		status = FLASH_GetStatus();
		Timeout--;
	}
	
	if(Timeout == 0x00 )
	{
		status = FLASH_TIMEOUT;
	}
	/* Return the operation status */
	return status;
}	

static FLASH_Status FLASH_OB_RDPConfig(uint8_t OB_RDP)
{
	FLASH_Status status = FLASH_COMPLETE;
	// Get the actual read protection Option Byte value 
	// if ((unsigned char)(FLASH->OBR & (FLASH_OBR_RDPRT1 | FLASH_OBR_RDPRT2)) != RESET)
	status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
	if(status == FLASH_COMPLETE)
	{
		FLASH->OPTR |= OB_RDP;			
		FLASH->CR |= FLASH_CR_OPTSTRT;
		// Wait for last operation to be completed
		status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);   
		FLASH->CR &= ~FLASH_CR_OPTSTRT;
	}
	return status;
}

void EnbleReadProtection(void)
{	
	if ((FLASH->OPTR&0xff) == OB_RDP_LEVEL_0)
	{
		while (FLASH->SR&FLASH_SR_BSY1);
		FLASH_Unlock();
		FLASH_OB_Unlock();
		FLASH_OB_RDPConfig(OB_RDP_LEVEL_1);
		FLASH_OB_Launch();
		FLASH_OB_Lock();
		FLASH_Lock();
		return;
	}
}
#elif defined(PN_GMR)

#include <stm32f0xx.h>
#include "Flash.h"
#include "System.h"

#define FLASH_ER_PRG_TIMEOUT    ((uint32_t)0x000B0000)
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000800)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08009000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08020000)   /* End @ of user Flash area */


static void FLASH_Unlock(void)
{
	if((FLASH->CR & FLASH_CR_LOCK) != RESET)
	{
		/* Unlocking the program memory access */
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
}

static void FLASH_Lock(void)
{
	/* Set the LOCK Bit to lock the FLASH control register and program memory access */
	FLASH->CR |= FLASH_CR_LOCK;
}

static void FLASH_OB_Unlock(void)
{
	//If option byte write is disabled,enable it
	if((FLASH->CR & FLASH_CR_OPTWRE) == RESET)
	{ 
		// Unlocking the option bytes block access
		FLASH->OPTKEYR = FLASH_OPTKEY1;
		FLASH->OPTKEYR = FLASH_OPTKEY2;
	}
}

static void FLASH_OB_Lock(void)
{
	//Set the OPTWREN Bit to lock the option bytes block access 
	FLASH->CR &= ~FLASH_CR_OPTWRE;
}

static void FLASH_OB_Launch(void)
{
	//Set the OBL_Launch bit to launch the option byte loading 
	FLASH->CR |= FLASH_CR_OBL_LAUNCH;
}

static FLASH_Status FLASH_GetStatus(void)
{
	FLASH_Status FLASHstatus = FLASH_COMPLETE;
	
	if((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) 
	{
		FLASHstatus = FLASH_BUSY;
	}
	else 
	{  
		if((FLASH->SR & (uint32_t)FLASH_SR_WRPERR)!= (uint32_t)0x00)
		{ 
			FLASHstatus = FLASH_ERROR_WRP;
		}
		else 
		{
			if((FLASH->SR & (uint32_t)(FLASH_SR_PGERR)) != (uint32_t)0x00)
			{
				FLASHstatus = FLASH_ERROR_PROGRAM; 
			}
			else
			{
				FLASHstatus = FLASH_COMPLETE;
			}
		}
	}
	/* Return the FLASH Status */
	return FLASHstatus;
}

static FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
	FLASH_Status status = FLASH_COMPLETE;
	 
	/* Check for the FLASH Status */
	status = FLASH_GetStatus();
	
	/* Wait for a FLASH operation to complete or a TIMEOUT to occur */
	while((status == FLASH_BUSY) && (Timeout != 0x00))
	{
		status = FLASH_GetStatus();
		Timeout--;
	}
	
	if(Timeout == 0x00 )
	{
		status = FLASH_TIMEOUT;
	}
	/* Return the operation status */
	return status;
}

static FLASH_Status FLASH_OB_RDPConfig(uint8_t OB_RDP)
{
	FLASH_Status status = FLASH_COMPLETE;
	// Get the actual read protection Option Byte value 
	// if ((unsigned char)(FLASH->OBR & (FLASH_OBR_RDPRT1 | FLASH_OBR_RDPRT2)) != RESET)
	status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
	if(status == FLASH_COMPLETE)
	{
		FLASH->CR |= FLASH_CR_OPTER;
		FLASH->CR |= FLASH_CR_STRT;  
		// Wait for last operation to be completed 
		status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);   
		if(status == FLASH_COMPLETE)
		{
			// If the erase operation is completed, disable the OPTER Bit 
			FLASH->CR &= ~FLASH_CR_OPTER;     
			// Enable the Option Bytes Programming operation 
			FLASH->CR |= FLASH_CR_OPTPG;     
			OB->RDP = OB_RDP;
		OB->USER=0xFF;
			// Wait for last operation to be completed
			status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);   
		//Disable the OPTPG Bit
		FLASH->CR &= ~FLASH_CR_OPTPG;
		}
	}
	return status;
}

void EnbleReadProtection(void)
{
	if ((OB->RDP&0xff) == OB_RDP_LEVEL_0)
	{
		while (FLASH->SR&FLASH_SR_BSY);
		FLASH_Unlock();
		FLASH_OB_Unlock();
		FLASH_OB_RDPConfig(OB_RDP_LEVEL_1);
		FLASH_OB_Lock();
		FLASH_Lock();
		FLASH_OB_Launch();
	}
	return;
}

#endif

