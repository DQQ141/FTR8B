#if !defined(UPDATE_H)
#define UPDATE_H

#include "System definitions.h"

// Company codes
#define UPD_CC_FLYSKY     0x0001
#define UPD_CC_KY_EVO     0x0007
#define UPD_MAKE_PRODUCT_NUMBER(CompanyCode,ProductNumber) ((CompanyCode<<16)|ProductNumber)
// Product codes
#define RFMPN_FS_FTR10 	  0x12340002
#define RFMPN_FS_FGR4  	  0x12340004
#define RFMPN_FS_FGR4S 	  0x12340006

#define UPD_PN_FTR10      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0100)
#define UPD_PN_FGR4       UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0101)
#define UPD_PN_FGR4S      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0102)
#define UPD_PN_FTR16S     UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0103)
#define UPD_PN_MINIZ      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0104)
#define UPD_PN_KY_MINIZ   UPD_MAKE_PRODUCT_NUMBER(UPD_CC_KY_EVO    ,0x0104)
#define UPD_PN_FTR8B      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0105)
#define UPD_PN_FTR12B     UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0106)
#define UPD_PN_FGR8B      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0107)
#define UPD_PN_FGR12B     UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0108)
#define UPD_PN_GMR        UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0109)
#define UPD_PN_TMR        UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x010A)
#define UPD_PN_INR4_GYB   UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x010B)
#define UPD_PN_INR6_HS    UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x010C)
#define UPD_PN_FTR4B      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x010D)
#define UPD_PN_FGR4B      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x010E)
#define UPD_PN_FBR12      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x010F)
#define UPD_PN_INR6_FC    UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0110)
#define UPD_PN_TR8B       UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0111)
#define UPD_PN_FBR8       UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0112)
#define UPD_PN_FBR4       UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0113)
#define UPD_PN_FGR4D      UPD_MAKE_PRODUCT_NUMBER(UPD_CC_FLYSKY    ,0x0114)
#if defined(PHY_SX1280)
	#define LNK_UPD_TX_POWER             0 // In 0.25dBm unit
	#define LNK_UPD_TX_TIMING_PRE_TX   400
	#define LNK_UPD_TX_TIMING_TX       696
	#define LNK_UPD_TX_TIMING_POST_TX  245
	#define LNK_UPD_TX_TIMING_PRE_RX   400
	#define LNK_UPD_TX_TIMING_RX_IDLE  195
	#define LNK_UPD_RX_TIMING_PRE_TX   318
	#define LNK_UPD_RX_TIMING_TX       417
	#define LNK_UPD_RX_TIMING_PRE_RX   561
	#define LNK_UPD_RX_TIMING_RX_IDLE  300
	#define LNK_UPD_RX_MARGIN_TIME     100 // Time margin before and after reception for both transmitter and receiver
	#define LNK_UPDATE_CHANNEL_NB      (-81*1000*10) // 2481MHz
	#define LNK_UPDATE_TIMEOUT         1000
#endif

#define LNK_UPD_CMD_NONE        0x00
#define LNK_UPD_CMD_GET_VERSION 0x01
#define LNK_UPD_CMD_ERASE_PAGE  0x02
#define LNK_UPD_CMD_WRITE_DATA  0x03
#define LNK_UPD_CMD_CALC_CRC32  0x04
#define LNK_UPD_CMD_REBOOT      0x05

#define LNK_UPD_DATA_BLOCK_LENGTH 32
#define LNK_UPD_NB_DATA_BLOCKS     8
#define LNK_UPD_DATA_LENGTH       (LNK_UPD_DATA_BLOCK_LENGTH*LNK_UPD_NB_DATA_BLOCKS)
#define LNK_UPD_CONFIG_ADDRESS    0x80000000
#if defined(UPD_PN_FTR10) || defined(UPD_PN_FGR4) || defined(UPD_PN_FGR4S) || defined(UPD_PN_FTR16S) || defined(PN_GMR)
	#define LNK_UPD_PAGE_LENGTH       0x0400
#elif defined(UPD_PN_FTR8B) ||  defined(UPD_PN_FTR12B)
	#define LNK_UPD_PAGE_LENGTH       0x0800
#endif

// Update commands
typedef struct __attribute__((packed))
{
	unsigned char Command;
} sLNK_UC_ByteCommand;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long PageAddress; // 0x80000000 for configuration page
} sLNK_UC_ErasePage;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long BaseAddress; // 0x80000000 and up for configuration page
	unsigned char DataBlockNb; // From 0 to LNK_UPD_NB_DATA_BLOCKS-1
	unsigned char DataBlock[LNK_UPD_DATA_BLOCK_LENGTH];
} sLNK_UC_WriteData;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long Address; // 0x80000000 and up for configuration page
	unsigned long Length;
} sLNK_UC_CalcCRC32;

typedef union __attribute__((packed))
{
	unsigned char Command;
	sLNK_UC_ByteCommand ByteCommand;
	sLNK_UC_ErasePage ErasePage;
	sLNK_UC_WriteData WriteData;
	sLNK_UC_CalcCRC32 CalcCRC32;
} sLNK_UpdateCommand;

// Update responses
typedef struct __attribute__((packed))
{
	unsigned char Command;
} sLNK_UR_ByteResponse;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long ProductNumber;
	unsigned short FirmwareVersion;
	unsigned long FirmwareLength;
	unsigned long FirmwareCRC32;
} sLNK_UR_GetVersion;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long PageAddress; // 0x80000000 for configuration page
	BOOL Success;
} sLNK_UR_ErasePage;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long BaseAddress; // 0x80000000 and up for configuration page
	unsigned char ReceivedDataBlocks; // One bit per block, 0 if error
} sLNK_UR_WriteData;

typedef struct __attribute__((packed))
{
	unsigned char Command;
	unsigned long Address; // 0x80000000 and up for configuration page
	unsigned long Length;
	unsigned long CRC32;
} sLNK_UR_CalcCRC32;

typedef union __attribute__((packed))
{
	unsigned char Command;
	sLNK_UR_ByteResponse ByteResponse;
	sLNK_UR_ErasePage ErasePage;
	sLNK_UR_GetVersion GetVersion;
	sLNK_UR_WriteData WriteData;
	sLNK_UR_CalcCRC32 CalcCRC32;
} sLNK_UpdateResponse;

#endif // !defined(UPDATE_H)
