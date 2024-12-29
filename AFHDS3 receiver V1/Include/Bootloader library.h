#if !defined(BOOTLOADER_LIBRARY_H)
#define BOOTLOADER_LIBRARY_H

#include "System definitions.h"

typedef struct
{
	unsigned long ProductNumber;
	unsigned long FirmwareLength;
	unsigned long FirmwareCRC32;
	unsigned long FlashConfigBaseAddress;
	unsigned long FlashFirmwareBaseAddress;
	unsigned short FirmwareVersion;
	BOOL IsValidFirmwarePresent;
	unsigned char Padding;
} sBLL_Init;

extern volatile unsigned long SYS_SysTickMs;

void LNKTIM_TimerCompareIRQHandler(void);
void BootloaderLibrary(const sBLL_Init *pInit);

void APP_SystemReboot(void);

BOOL FLH_ErasePage(unsigned long PageAddress);
void FLH_Lock(void);
BOOL FLH_Program(unsigned long ProgramAddress,const void *pData,unsigned long DataLength);
BOOL FLH_ProgramWord(unsigned long ProgramAddress,unsigned short ProgramWord);
void FLH_Unlock(void);

#endif // !defined(BOOTLOADER_LIBRARY_H)
