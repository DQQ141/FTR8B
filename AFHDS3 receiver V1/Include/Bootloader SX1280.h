#if !defined(BOOTLOADER_SX1280_H)
#define BOOTLOADER_SX1280_H

#include "System definitions.h"

typedef struct
{
	unsigned long RXDuration;
	BOOL ResumeRXOnFailure;
	unsigned char Padding[3];
} sPHY_RXConfig;

void PHY_SX1280_DIO1IRQHandler(void);

BOOL PHY_SX1280_IsBusy(void);
void PHY_SX1280_SetResetPin(BOOL Level);

void SPI_TransferWait(const void *pTXData,void *pRXData,unsigned long DataLength);

void SYS_DelayUs(unsigned long Delay_us);

#endif // !defined(BOOTLOADER_SX1280_H)
