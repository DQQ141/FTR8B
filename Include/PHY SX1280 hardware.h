#if !defined(PHY_SX1280_HARDWARE_H)
#define PHY_SX1280_HARDWARE_H

#include "System definitions.h"

#define PHY_SX1280_PREPARE_TX_BASE_TIME  130
#define PHY_SX1280_PREPARE_RX_TIME        90
#define PHY_SX1280_STBY_TO_FS_TIME        60
#define PHY_SX1280_STBY_TO_RX_TIME        90
#define PHY_SX1280_FS_TO_TX_TIME          27
#define PHY_SX1280_FS_TO_RX_TIME          34
#define PHY_SX1280_RX_TO_TX_TIME         (39+20) // 20=preamble detection busy time
#define PHY_SX1280_RX_TO_STBY_TIME       130
#define PHY_SX1280_DELAY_POST_TX          50
#define PHY_SX1280_BASE_DELAY_POST_RX     50
#define PHY_SX1280_TX_CCA_TIME            50

#define PHY_SX1280_RESET_TIME_MIN        100 // In microseconds

#define PHY_SX1280_CMD_GET_PACKETTYPE           0x03
#define PHY_SX1280_CMD_GET_IRQSTATUS            0x15
#define PHY_SX1280_CMD_GET_RXBUFFERSTATUS       0x17
#define PHY_SX1280_CMD_WRITE_REGISTER           0x18
#define PHY_SX1280_CMD_READ_REGISTER            0x19
#define PHY_SX1280_CMD_WRITE_BUFFER             0x1A
#define PHY_SX1280_CMD_READ_BUFFER              0x1B
#define PHY_SX1280_CMD_GET_PACKETSTATUS         0x1D
#define PHY_SX1280_CMD_GET_RSSIINST             0x1F
#define PHY_SX1280_CMD_SET_STANDBY              0x80
#define PHY_SX1280_CMD_SET_RX                   0x82
#define PHY_SX1280_CMD_SET_TX                   0x83
#define PHY_SX1280_CMD_SET_SLEEP                0x84
#define PHY_SX1280_CMD_SET_RFFREQUENCY          0x86
#define PHY_SX1280_CMD_SET_CADPARAMS            0x88
#define PHY_SX1280_CMD_CALIBRATE                0x89
#define PHY_SX1280_CMD_SET_PACKETTYPE           0x8A
#define PHY_SX1280_CMD_SET_MODULATIONPARAMS     0x8B
#define PHY_SX1280_CMD_SET_PACKETPARAMS         0x8C
#define PHY_SX1280_CMD_SET_DIOIRQPARAMS         0x8D
#define PHY_SX1280_CMD_SET_TXPARAMS             0x8E
#define PHY_SX1280_CMD_SET_BUFFERBASEADDRESS    0x8F
#define PHY_SX1280_CMD_SET_RXDUTYCYCLE          0x94
#define PHY_SX1280_CMD_SET_REGULATORMODE        0x96
#define PHY_SX1280_CMD_CLR_IRQSTATUS            0x97
#define PHY_SX1280_CMD_SET_AUTOTX               0x98
#define PHY_SX1280_CMD_SET_LONGPREAMBLE         0x9B
#define PHY_SX1280_CMD_SET_UARTSPEED            0x9D
#define PHY_SX1280_CMD_SET_AUTOFS               0x9E
#define PHY_SX1280_CMD_SET_RANGING_ROLE         0xA3
#define PHY_SX1280_CMD_GET_STATUS               0xC0
#define PHY_SX1280_CMD_SET_FS                   0xC1
#define PHY_SX1280_CMD_SET_CAD                  0xC5
#define PHY_SX1280_CMD_SET_TXCONTINUOUSWAVE     0xD1
#define PHY_SX1280_CMD_SET_TXCONTINUOUSPREAMBLE 0xD2
#define PHY_SX1280_CMD_SET_SAVECONTEXT          0xD5

#define PHY_SX1280_IRQ_RADIO_NONE                      0x0000
#define PHY_SX1280_IRQ_TX_DONE                         0x0001
#define PHY_SX1280_IRQ_RX_DONE                         0x0002
#define PHY_SX1280_IRQ_SYNCWORD_VALID                  0x0004
#define PHY_SX1280_IRQ_SYNCWORD_ERROR                  0x0008
#define PHY_SX1280_IRQ_HEADER_VALID                    0x0010
#define PHY_SX1280_IRQ_HEADER_ERROR                    0x0020
#define PHY_SX1280_IRQ_CRC_ERROR                       0x0040
#define PHY_SX1280_IRQ_RANGING_SLAVE_RESPONSE_DONE     0x0080
#define PHY_SX1280_IRQ_RANGING_SLAVE_REQUEST_DISCARDED 0x0100
#define PHY_SX1280_IRQ_RANGING_MASTER_RESULT_VALID     0x0200
#define PHY_SX1280_IRQ_RANGING_MASTER_TIMEOUT          0x0400
#define PHY_SX1280_IRQ_RANGING_SLAVE_REQUEST_VALID     0x0800
#define PHY_SX1280_IRQ_CAD_DONE                        0x1000
#define PHY_SX1280_IRQ_CAD_DETECTED                    0x2000
#define PHY_SX1280_IRQ_RX_TX_TIMEOUT                   0x4000
#define PHY_SX1280_IRQ_PREAMBLE_DETECTED               0x8000
#define PHY_SX1280_IRQ_RADIO_ALL                       0xFFFF

#define PHY_SX1280_PA_RAMP_02_US 0x00
#define PHY_SX1280_PA_RAMP_04_US 0x20
#define PHY_SX1280_PA_RAMP_06_US 0x40
#define PHY_SX1280_PA_RAMP_08_US 0x60
#define PHY_SX1280_PA_RAMP_10_US 0x80
#define PHY_SX1280_PA_RAMP_12_US 0xA0
#define PHY_SX1280_PA_RAMP_16_US 0xC0
#define PHY_SX1280_PA_RAMP_20_US 0xE0

#define PHY_SX1280_TX_BUFFER_BASE_ADDRESS 0
#define PHY_SX1280_RX_BUFFER_BASE_ADDRESS 0x80

typedef enum
{
	PACKET_TYPE_PHY_SX1280_GFSK = 0x00,
	PACKET_TYPE_PHY_SX1280_LORA,
	PACKET_TYPE_PHY_SX1280_RANGING,
	PACKET_TYPE_PHY_SX1280_FLRC
} ePHY_SX1280_PacketType;

typedef enum
{
	PHY_SX1280_GFSK_BR_2_000_BW_2_4 = 0x04,
	PHY_SX1280_GFSK_BR_1_600_BW_2_4 = 0x28,
	PHY_SX1280_GFSK_BR_1_000_BW_2_4 = 0x4C,
	PHY_SX1280_GFSK_BR_1_000_BW_1_2 = 0x45,
	PHY_SX1280_GFSK_BR_0_800_BW_2_4 = 0x70,
	PHY_SX1280_GFSK_BR_0_800_BW_1_2 = 0x69,
	PHY_SX1280_GFSK_BR_0_500_BW_1_2 = 0x8D,
	PHY_SX1280_GFSK_BR_0_500_BW_0_6 = 0x86,
	PHY_SX1280_GFSK_BR_0_400_BW_1_2 = 0xB1,
	PHY_SX1280_GFSK_BR_0_400_BW_0_6 = 0xAA,
	PHY_SX1280_GFSK_BR_0_250_BW_0_6 = 0xCE,
	PHY_SX1280_GFSK_BR_0_250_BW_0_3 = 0xC7,
	PHY_SX1280_GFSK_BR_0_125_BW_0_3 = 0xEF,
} ePHY_SX1280_GFSK_BitrateBandwidth;

typedef enum
{
	PHY_SX1280_GFSK_MOD_IND_0_35 =  0,
	PHY_SX1280_GFSK_MOD_IND_0_50 =  1,
	PHY_SX1280_GFSK_MOD_IND_0_75 =  2,
	PHY_SX1280_GFSK_MOD_IND_1_00 =  3,
	PHY_SX1280_GFSK_MOD_IND_1_25 =  4,
	PHY_SX1280_GFSK_MOD_IND_1_50 =  5,
	PHY_SX1280_GFSK_MOD_IND_1_75 =  6,
	PHY_SX1280_GFSK_MOD_IND_2_00 =  7,
	PHY_SX1280_GFSK_MOD_IND_2_25 =  8,
	PHY_SX1280_GFSK_MOD_IND_2_50 =  9,
	PHY_SX1280_GFSK_MOD_IND_2_75 = 10,
	PHY_SX1280_GFSK_MOD_IND_3_00 = 11,
	PHY_SX1280_GFSK_MOD_IND_3_25 = 12,
	PHY_SX1280_GFSK_MOD_IND_3_50 = 13,
	PHY_SX1280_GFSK_MOD_IND_3_75 = 14,
	PHY_SX1280_GFSK_MOD_IND_4_00 = 15,
} ePHY_SX1280_GFSK_ModulationIndex;

typedef enum
{
	PHY_SX1280_MOD_SHAPING_BT_OFF = 0x00,		 //! No filtering
	PHY_SX1280_MOD_SHAPING_BT_1_0 = 0x10,
	PHY_SX1280_MOD_SHAPING_BT_0_5 = 0x20,
} ePHY_SX1280_ModulationShaping;

typedef struct __attribute__((packed))
{
	ePHY_SX1280_GFSK_BitrateBandwidth BitrateBandwidth;
	ePHY_SX1280_GFSK_ModulationIndex ModulationIndex;
	ePHY_SX1280_ModulationShaping ModulationShaping;
} sPHY_SX1280_ModulationParamsGFSK;

typedef enum
{
	PHY_SX1280_FLRC_BR_1_300_BW_1_2 = 0x45,
	PHY_SX1280_FLRC_BR_1_040_BW_1_2 = 0x69,
	PHY_SX1280_FLRC_BR_0_650_BW_0_6 = 0x86,
	PHY_SX1280_FLRC_BR_0_520_BW_0_6 = 0xAA,
	PHY_SX1280_FLRC_BR_0_325_BW_0_3 = 0xC7,
	PHY_SX1280_FLRC_BR_0_260_BW_0_3 = 0xEB,
} ePHY_SX1280_FLRC_BitrateBandwidth;

typedef enum
{
	PHY_SX1280_FLRC_CR_1_2 = 0x00,
	PHY_SX1280_FLRC_CR_3_4 = 0x02,
	PHY_SX1280_FLRC_CR_1_0 = 0x04,
} ePHY_SX1280_FLRC_CodingRate;

typedef struct __attribute__((packed))
{
	ePHY_SX1280_FLRC_BitrateBandwidth BitrateBandwidth;
	ePHY_SX1280_FLRC_CodingRate CodingRate;
	ePHY_SX1280_ModulationShaping ModulationShaping;
} sPHY_SX1280_ModulationParamsFLRC;

typedef enum
{
	PHY_SX1280_LORA_SF5  = 0x50,
	PHY_SX1280_LORA_SF6  = 0x60,
	PHY_SX1280_LORA_SF7  = 0x70,
	PHY_SX1280_LORA_SF8  = 0x80,
	PHY_SX1280_LORA_SF9  = 0x90,
	PHY_SX1280_LORA_SF10 = 0xA0,
	PHY_SX1280_LORA_SF11 = 0xB0,
	PHY_SX1280_LORA_SF12 = 0xC0,
} ePHY_SX1280_LoRa_SpreadingFactor;

typedef enum
{
	PHY_SX1280_LORA_BW_0200 = 0x34,
	PHY_SX1280_LORA_BW_0400 = 0x26,
	PHY_SX1280_LORA_BW_0800 = 0x18,
	PHY_SX1280_LORA_BW_1600 = 0x0A,
} ePHY_SX1280_LoRa_Bandwidth;

typedef enum
{
	PHY_SX1280_LORA_CR_4_5    = 0x01,
	PHY_SX1280_LORA_CR_4_6    = 0x02,
	PHY_SX1280_LORA_CR_4_7    = 0x03,
	PHY_SX1280_LORA_CR_4_8    = 0x04,
	PHY_SX1280_LORA_CR_LI_4_5 = 0x05,
	PHY_SX1280_LORA_CR_LI_4_6 = 0x06,
	PHY_SX1280_LORA_CR_LI_4_7 = 0x07,
} ePHY_SX1280_LoRa_CodingRate;

typedef struct __attribute__((packed))
{
	ePHY_SX1280_LoRa_SpreadingFactor SpreadingFactor;
	ePHY_SX1280_LoRa_Bandwidth Bandwidth;
	ePHY_SX1280_LoRa_CodingRate CodingRate;
} sPHY_SX1280_ModulationParamsLoRa_Ranging;

typedef union __attribute__((packed))
{
	sPHY_SX1280_ModulationParamsGFSK GFSK;
	sPHY_SX1280_ModulationParamsFLRC FLRC;
	sPHY_SX1280_ModulationParamsLoRa_Ranging LoRa_Ranging;
} sPHY_SX1280_ModulationParams;

typedef enum
{
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_04_BITS = 0x00,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_08_BITS = 0x10,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_12_BITS = 0x20,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_16_BITS = 0x30,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_20_BITS = 0x40,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_24_BITS = 0x50,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_28_BITS = 0x60,
	PHY_SX1280_GFSK_PREAMBLE_LENGTH_32_BITS = 0x70,
} ePHY_SX1280_GFSK_PreambleLength;

typedef enum
{
	PHY_SX1280_GFSK_SYNCWORD_LENGTH_1_BYTE = 0x00,
	PHY_SX1280_GFSK_SYNCWORD_LENGTH_2_BYTE = 0x02,
	PHY_SX1280_GFSK_SYNCWORD_LENGTH_3_BYTE = 0x04,
	PHY_SX1280_GFSK_SYNCWORD_LENGTH_4_BYTE = 0x06,
	PHY_SX1280_GFSK_SYNCWORD_LENGTH_5_BYTE = 0x08,
} ePHY_SX1280_GFSK_SyncWordLength;

typedef enum
{
	PHY_SX1280_GFSK_FLRC_SYNCWORD_OFF   = 0x00,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_1     = 0x10,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_2     = 0x20,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_1_2   = 0x30,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_3     = 0x40,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_1_3   = 0x50,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_2_3   = 0x60,
	PHY_SX1280_GFSK_FLRC_SYNCWORD_1_2_3 = 0x70,
} ePHY_SX1280_GFSK_FLRC_SyncWordCombination;

typedef enum
{
	PHY_SX1280_GFSK_FLRC_PACKET_FIXED_LENGTH    = 0x00,
	PHY_SX1280_GFSK_FLRC_PACKET_VARIABLE_LENGTH = 0x20,
} ePHY_SX1280_GFSK_FLRC_PacketLengthMode;

typedef enum
{
	PHY_SX1280_GFSK_CRC_OFF     = 0x00,
	PHY_SX1280_GFSK_CRC_1_BYTES = 0x10,
	PHY_SX1280_GFSK_CRC_2_BYTES = 0x20,
} ePHY_SX1280_GFSK_CRCLength;

typedef enum
{
	PHY_SX1280_GFSK_WHITENING_ON  = 0x00,
	PHY_SX1280_GFSK_WHITENING_OFF = 0x08,
} ePHY_SX1280_GFSK_WhiteningMode;

typedef struct __attribute__((packed))
{
	ePHY_SX1280_GFSK_PreambleLength PreambleLength;
	ePHY_SX1280_GFSK_SyncWordLength SyncWordLength;
	ePHY_SX1280_GFSK_FLRC_SyncWordCombination SyncWordCombination;
	ePHY_SX1280_GFSK_FLRC_PacketLengthMode PacketLengthMode;
	unsigned char PayloadLength; // 0 to 255 bytes, ignored and copied from PayloadLength
	ePHY_SX1280_GFSK_CRCLength CRCLength;
	ePHY_SX1280_GFSK_WhiteningMode WhiteningMode;
} sPHY_SX1280_PacketParamsGFSK;

typedef enum
{
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_08_BITS = 0x10,
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_12_BITS = 0x20,
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_16_BITS = 0x30,
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_20_BITS = 0x40,
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_24_BITS = 0x50,
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_28_BITS = 0x60,
	PHY_SX1280_FLRC_PREAMBLE_LENGTH_32_BITS = 0x70,
} ePHY_SX1280_FLRC_PreambleLength;

typedef enum
{
	PHY_SX1280_FLRC_SYNC_NOSYNC        = 0x00, // 21 bits preamble
	PHY_SX1280_FLRC_SYNC_WORD_LEN_P32S = 0x04, // 21 bits preamble + 32 bits sync word
} ePHY_SX1280_FLRC_SyncWordLength;

typedef enum
{
	PHY_SX1280_FLRC_CRC_OFF     = 0x00,
	PHY_SX1280_FLRC_CRC_1_BYTES = 0x10,
	PHY_SX1280_FLRC_CRC_2_BYTES = 0x20,
	PHY_SX1280_FLRC_CRC_3_BYTES = 0x30,
} ePHY_SX1280_FLRC_CRCLength;

typedef enum
{
	PHY_SX1280_FLRC_WHITENING_OFF = 0x08,
} ePHY_SX1280_FLRC_WhiteningMode;

typedef struct __attribute__((packed))
{
	ePHY_SX1280_FLRC_PreambleLength PreambleLength;
	ePHY_SX1280_FLRC_SyncWordLength SyncWordLength;
	ePHY_SX1280_GFSK_FLRC_SyncWordCombination SyncWordCombination;
	ePHY_SX1280_GFSK_FLRC_PacketLengthMode PacketLengthMode;
	unsigned char PayloadLength; // 6 to 127 bytes, ignored and copied from PayloadLength
	ePHY_SX1280_FLRC_CRCLength CRCLength;
	ePHY_SX1280_FLRC_WhiteningMode WhiteningMode;
} sPHY_SX1280_PacketParamsFLRC;

typedef enum
{
	PHY_SX1280_LORA_EXPLICIT_HEADER = 0x00,
	PHY_SX1280_LORA_IMPLICIT_HEADER = 0x80,
} ePHY_SX1280_LoRa_HeaderMode;

typedef enum
{
	PHY_SX1280_LORA_CRC_ON  = 0x20,
	PHY_SX1280_LORA_CRC_OFF = 0x00,
} ePHY_SX1280_LoRa_CRCMode;

typedef enum
{
	PHY_SX1280_LORA_IQ_NORMAL   = 0x40,
	PHY_SX1280_LORA_IQ_INVERTED = 0x00,
} ePHY_SX1280_LoRa_IQMode;

typedef struct __attribute__((packed))
{
	unsigned char PreambleLengthMantissa:4;
	unsigned char PreambleLengthExponent:4;
	ePHY_SX1280_LoRa_HeaderMode HeaderMode;
	unsigned char PayloadLength; // 1 to 255 bytes, ignored and copied from PayloadLength
	ePHY_SX1280_LoRa_CRCMode CRCMode;
	ePHY_SX1280_LoRa_IQMode IQMode;
	unsigned char Reserved1;
	unsigned char Reserved2;
} sPHY_SX1280_PacketParamsLoRa_Ranging;

typedef union __attribute__((packed))
{
	sPHY_SX1280_PacketParamsGFSK GFSK;
	sPHY_SX1280_PacketParamsFLRC FLRC;
	sPHY_SX1280_PacketParamsLoRa_Ranging LoRa_Ranging;
} sPHY_SX1280_PacketParams;

typedef struct __attribute__((packed))
{
	unsigned char CommandNb;
	sPHY_SX1280_ModulationParams Params;
} sPHY_SX1280_SetModulationParamsCommand;

typedef struct __attribute__((packed))
{
	unsigned char CommandNb;
	sPHY_SX1280_PacketParams Params;
} sPHY_SX1280_SetPacketParamsCommand;

typedef struct __attribute__((packed))
{
	unsigned char CommandNb;
	ePHY_SX1280_PacketType Type;
} sPHY_SX1280_SetPacketTypeCommand;

#define PHY_SX1280_S_CS_COMMAND_PROCESSED 1
#define PHY_SX1280_S_CS_DATA_AVAILABLE    2
#define PHY_SX1280_S_CS_COMMAND_TIMEOUT   3
#define PHY_SX1280_S_CS_COMMAND_ERROR     4
#define PHY_SX1280_S_CS_COMMAND_FAILED    5
#define PHY_SX1280_S_CS_TX_DONE           6
#define PHY_SX1280_S_CM_SDTBY_RC   2
#define PHY_SX1280_S_CM_STDBY_XOSC 3
#define PHY_SX1280_S_CM_FS         4
#define PHY_SX1280_S_CM_RX         5
#define PHY_SX1280_S_CM_TX         6
typedef struct __attribute__((packed))
{
	unsigned char Busy:1;
	unsigned char Reserved:1;
	unsigned char CommandStatus:3;
	unsigned char CircuitMode:3;
} sPHY_SX1280_Status;

#endif // !defined(PHY_SX1280_HARDWARE_H)
