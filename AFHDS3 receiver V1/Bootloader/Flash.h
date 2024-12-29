#if !defined(FLASH_H)
#define FLASH_H

#include "System definitions.h"

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#define FLASH_KEY1 0x45670123U
	#define FLASH_KEY2 0xCDEF89ABU
#elif defined(PN_GMR)
#endif

BOOL FLH_ErasePage(unsigned long PageAddress);
void FLH_Lock(void);
BOOL FLH_Program(unsigned long ProgramAddress,const void *pData,unsigned long DataLength);
BOOL FLH_ProgramDoubleWord(unsigned long ProgramAddress,unsigned long Word1,unsigned long Word2);
#if defined(PN_GMR)
	BOOL FLH_ProgramHalfWord(unsigned long ProgramAddress,unsigned short HalfWord);
#endif
void FLH_Unlock(void);

#endif // !defined(FLASH_H)
