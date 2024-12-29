#define EEPROM_ADDRESS   0x50
#define EEPROM_PAGE_SIZE   16

void EEPROM_Init(void);
BOOL EEPROM_Read(unsigned long Address,void *pData,unsigned long DataLength);
BOOL EEPROM_Write(unsigned long Address,void *pData,unsigned long DataLength);
