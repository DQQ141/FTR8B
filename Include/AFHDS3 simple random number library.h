#if !defined(AFHDS3_SIMPLE_RANDOM_NUMBER_LIBRARY_H)
#define AFHDS3_SIMPLE_RANDOM_NUMBER_LIBRARY_H

#include "System definitions.h"

unsigned long SYS_GetSimpleRandomNumber(void);
unsigned long SYS_GetSimpleRandomNumberFromSeed(unsigned long Seed);

#endif // !defined(AFHDS3_SIMPLE_RANDOM_NUMBER_LIBRARY_H)
