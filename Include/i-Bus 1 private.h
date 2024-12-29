#if !defined(I_BUS_1_PRIVATE_H)
#define I_BUS_1_PRIVATE_H

#include "i-Bus.h"
#include "i-Bus 1.h"

#define IB1_USART_SPEED   115200 // In bauds
#define IB1_CYCLE_TIME      4000 // In microseconds

#define IBPARAM_SET_SERVO_CHANNEL 0x8001
#define IBPARAM_VOLT_SENSOR_CAL   0xC001

#define IB1_NB_MAX_ERRORS   10

#define IB1_ADDRESS_TIME 500 // Time in milliseconds between 2 IB1CMD_ADDRESS commands cycle
#define IB1_ADDRESS_MAX   15

#define IB1_RESET_TIMEOUT 250 // No command received timeout in milliseconds before a device resets itself

#define IB1CMD_ADDRESS   0x00UL
#define IB1CMD_GET_TYPE  0x01UL
#define IB1CMD_GET_VALUE 0x02UL
#define IB1CMD_SET_PARAM 0x03UL
#define IB1CMD_SERVO_OUT 0x04UL
#define IB1CMD_GET_PARAM 0x05UL
#define IB1CMD_RESET     0x07UL

#define IB1_MAKE_HEADER_SHORT(Length,Address,Command,IsSensorBus) \
	((unsigned short)(((unsigned long)Length) | (((unsigned long)Address)<<8) | (((unsigned long)Command)<<12) | (((unsigned long)IsSensorBus)<<15)))

typedef struct __attribute__((packed))
{
	unsigned char Length;
	unsigned char Address:4;
	unsigned char Command:3;
	unsigned char IsSensorBus:1;
} sIB1_Header;

typedef struct __attribute__((packed))
{
	sIB1_Header Header;
	unsigned short Checksum16;
} sIB1_NoDataPacket;

typedef union __attribute__((packed))
{
	unsigned char BlockNb:2;
	unsigned char Char[IB_MAX_VALUE_LENGTH];
	unsigned short Short[IB_MAX_VALUE_LENGTH/2];
	unsigned long Long[IB_MAX_VALUE_LENGTH/4];
} sIB1_DataBlock;

typedef struct __attribute__((packed))
{
	sIB1_Header Header;
	sIB1_DataBlock DataBlock;
	unsigned short Checksum16;
} sIB1_MaxDataPacket;

typedef struct __attribute__((packed))
{
	sIB1_Header Header;
	unsigned short ParamType;
	union __attribute__((packed))
	{
		unsigned char UChar;
		signed char SChar;
		unsigned short UShort;
		signed short SShort;
		unsigned long ULong;
		signed long SLong;
	} ParamValue;
	unsigned short Checksum16;
} sIB1_GetSetParam;

typedef struct __attribute__((packed))
{
	sIB1_Header Header;
	unsigned short Channels[IB1_NB_CHANNELS];
	unsigned short Checksum16;
} sIB1_CommandSetServosOutput;

typedef struct __attribute__((packed))
{
	sIB1_Header Header;
	eIB_DeviceType DeviceType;
	unsigned char DeviceValueLength;
	unsigned short Checksum16;
} sIB1_CommandGetTypeResponse;

typedef union __attribute__((packed))
{
	sIB1_Header Header;
	unsigned short HeaderShort; // Unsigned short coinciding with the header
	sIB1_NoDataPacket NoDataCmd;
	sIB1_GetSetParam GetSetParam;
	sIB1_CommandSetServosOutput SetServosOutput;
} sIB1_Command;

typedef union __attribute__((packed))
{
	sIB1_Header Header;
	unsigned short HeaderShort; // Unsigned short coinciding with the header
	sIB1_NoDataPacket NoData;
	sIB1_MaxDataPacket MaxData;
	sIB1_CommandGetTypeResponse GetType;
	sIB1_GetSetParam GetSetParam;
} sIB1_Response;

#endif // !defined(I_BUS_1_PRIVATE_H)
