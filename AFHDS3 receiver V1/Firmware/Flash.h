#ifndef __FLASH_H
#define __FLASH_H

#include "stdint.h"

// FLASH Read Protection Level  
#define     OB_RDP_LEVEL_0   ((unsigned char)0xAA)
#define     OB_RDP_LEVEL_1   ((unsigned char)0xBB)//except 0xAA & 0xCC
#define     OB_RDP_LEVEL_2   ((unsigned char)0xCC)	

typedef enum
{
	FLASH_BUSY = 1,
	FLASH_ERROR_WRP,
	FLASH_ERROR_PROGRAM,
	FLASH_COMPLETE,
	FLASH_TIMEOUT
}FLASH_Status;

void EnbleReadProtection(void);

	
#endif


