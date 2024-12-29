#if !defined(AFHDS3_CRC_LIBRARY_H)
#define AFHDS3_CRC_LIBRARY_H

#include "System definitions.h"

unsigned char SYS_CalcCRC8Table(const void *pData,unsigned long DataLength);
unsigned long SYS_CalcCRC32(const void *pData,unsigned long DataLength);
unsigned long SYS_CalcCRC32Table(const void *pData,unsigned long DataSize);

#endif // !defined(AFHDS3_CRC_LIBRARY_H)
