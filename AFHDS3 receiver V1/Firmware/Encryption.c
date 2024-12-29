#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D) 
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif
#include "string.h"

#include "System definitions.h"
#include "System receiver.h"
#include "Encryption library.h"

#include "Encryption.h"


void Encryption_Check(void)
{
	unsigned long  OutputCode[3];
		
	Encrypt_GenerateCode((unsigned long*)(UID_BASE1+UID_BASE2),OutputCode,96);
	if(memcmp(OutputCode,(const unsigned long  *)&EncryptionCode1,12))
	{
		while(memcmp(OutputCode,(const unsigned long  *)&EncryptionCode1,12))
		{
			while(1);
		}		
	}
	return;
}





