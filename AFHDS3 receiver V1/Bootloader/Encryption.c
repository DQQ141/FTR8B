#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

	#include "string.h"

	#include "System definitions.h"
	#include "System receiver.h"
	#include "Flash.h"
	#include "Encryption library.h"

	#include "Encryption.h"

#if defined (PN_GMR)
	const unsigned long  EncryptionFlag __attribute__((section(".ARM.__at_0x08002400"),used))=ENCRYPTION_SIGNATURE;
#else
	const unsigned long  EncryptionFlag __attribute__((section(".ARM.__at_0x08004800"),used))=ENCRYPTION_SIGNATURE;
#endif
	void Encrypt_WriteCode(void)
	{
		unsigned long  OutputCode[3];
		
		// Initialization parameter
		memset(OutputCode,0x00,12);	
		// Generate Password
		Encrypt_GenerateCode((unsigned long*)UID_BASE,OutputCode,96);
		// Write Flash
		FLH_Unlock();
		FLH_ErasePage(ENCRYPTION_DATA_ADDRESS);	
		FLH_Program(ENCRYPTION_DATA_ADDRESS,OutputCode,12);
		FLH_Lock();
		return;
	}

