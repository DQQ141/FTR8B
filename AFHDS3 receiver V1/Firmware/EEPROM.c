#include <string.h>

#include "System definitions.h"
#include "System.h"

#include "I2C.h"

#include "EEPROM.h"

void EEPROM_Init(void)
{
	return;
}


BOOL EEPROM_Read(unsigned long Address,void *pData,unsigned long DataLength)
{
    unsigned char EEPROMAddress;

    *(unsigned char *)pData=(unsigned char)Address&0xFF;
    EEPROMAddress=(unsigned char)(EEPROM_ADDRESS+(Address>>8));
    if (!I2C_Write(EEPROMAddress,pData,1,FALSE) ||
            !I2C_Read(EEPROMAddress,pData,DataLength,TRUE))
            return FALSE;
    else
            return TRUE;
}


BOOL EEPROM_Write(unsigned long Address,void *pData,unsigned long DataLength)
{
	unsigned char Buffer[1+EEPROM_PAGE_SIZE];

	while (DataLength)
	{
		unsigned char EEPROMAddress;
		unsigned long PageLength;
		
		PageLength=EEPROM_PAGE_SIZE-(Address&(EEPROM_PAGE_SIZE-1));
		if (PageLength>DataLength)
			PageLength=DataLength;
		Buffer[0]=(unsigned char)Address;
		EEPROMAddress=(unsigned char)(EEPROM_ADDRESS+(Address>>8));
		memcpy(Buffer+1,pData,PageLength);
		if (!I2C_Write(EEPROMAddress,Buffer,PageLength+1,TRUE))
			return FALSE;
		while (!I2C_Read(EEPROMAddress,Buffer,1,TRUE));
		Address+=PageLength;
		pData=(unsigned char *)pData+PageLength;
		DataLength-=PageLength;
	}
	return TRUE;
}






