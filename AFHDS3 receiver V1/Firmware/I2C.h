#if !defined(I2C_H)
#define I2C_H

#include "System definitions.h"

#define I2C_SPEED 400000 // 400KHz, 2500ns cycle
// I2C clock is SYSCLK
#define I2C_SCK_LOW 1600 // SCK minimum low time in nanoseconds (originally 1300ns)
#define I2C_SCL_DEL  200 // Delay between SDA edge and SCL rising in nanoseconds
#define I2C_SDA_DEL  100 // Delay between SCL falling and SDA edge in nanoseconds

#define I2C_NS_TO_TSCK(t) ((t*(SYSCLK/1000000)+999)/1000) // Converts a time in nanoseconds to a rounded up number of SYSCLK cycles

#define I2C_SCK_TSCK ((SYSCLK+I2C_SPEED-1)/(I2C_SPEED)) // SCK minimum cycle time in SYSCLK cycles
#define I2C_SCK_LOW_TSCK I2C_NS_TO_TSCK(I2C_SCK_LOW) // SCK minimum low time in SYSCLK cycles
#define I2C_SCK_HIGH_TSCK (I2C_SCK_TSCK-I2C_SCK_LOW_TSCK) // SCK minimum low time in SYSCLK cycles
#define I2C_SCL_DEL_TSCK I2C_NS_TO_TSCK(I2C_SCL_DEL)
#define I2C_SDA_DEL_TSCK I2C_NS_TO_TSCK(I2C_SDA_DEL)

void I2C_Init(void);
BOOL I2C_Read(unsigned char Address,void *pData,unsigned long DataLength,BOOL SendStop);
BOOL I2C_Transfer(BOOL IsWrite,unsigned char Address,void *pData,unsigned long DataLength,BOOL SendStop);
BOOL I2C_Write(unsigned char Address,const void *pData,unsigned long DataLength,BOOL SendStop);

#endif // !defined(I2C_H)
