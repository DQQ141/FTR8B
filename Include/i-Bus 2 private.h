#if !defined(I_BUS_2_PRIVATE_H)
#define I_BUS_2_PRIVATE_H

#include "System definitions.h"

#include "i-Bus.h"
#include "i-Bus 2.h"
#include "SES AFHDS3 external.h"

// Packet types
#define IB2_PT_CHANNELS      0
#define IB2_PT_COMMAND       1
#define IB2_PT_RESPONSE      2

// Packet subtypes for channels packet
#define IB2_PST_CHANNELS      0
#define IB2_PST_CHANNELS_TYPE 1
#define IB2_PST_FAILSAFE      2

#define IB2_CMD_RESET     0
#define IB2_CMD_GET_TYPE  1
#define IB2_CMD_GET_VALUE 2
#define IB2_CMD_GET_PARAM 3
#define IB2_CMD_SET_PARAM 4

#define IB2_NB_MAX_HUB_PORTS 7

#define IB2_NB_RETRIES 10

#define IB2_ENUMERATION_TIME_GAP 500 // Time between the end of an enumeration and the start of the next enumeration

#define IB2_SEND_RECEIVER_INTERNAL_SENSORS_DELAY 500 // In milliseconds

#define IB2_TIMING_CHANNELS_MAX              240
#define IB2_TIMING_CHANNELS_TO_COMMAND       100 // Time for the hubs to decode the channels packet address and switch their outputs
#define IB2_TIMING_COMMAND                   140
#define IB2_TIMING_COMMAND_MARGIN             50
#define IB2_TIMING_COMMAND_TO_RESPONSE        50 // Time for the hubs and devices to parse the command packet
#define IB2_TIMING_COMMAND_TO_RESPONSE_SW_1   30 // Time while the level 1 hub switches from output to input
#define IB2_TIMING_COMMAND_TO_RESPONSE_SW_2   30 // Time while the level 2 hubs switch from output to input
#define IB2_TIMING_RESPONSE                  140
#define IB2_TIMING_RESPONSE_MARGIN            50
#define IB2_TIMING_RESPONSE_TO_CHANNELS_SW_1  30 // Time while the level 2 hubs switch from input to output
#define IB2_TIMING_RESPONSE_TO_CHANNELS_SW_2  30 // Time while the level 1 hub switches from input to output
#define IB2_TIMING_IDLE                       50 // Idle time between 2 i-Bus 2 cycles

#define IB2_MAX_IDLE_TIME 10000 // Maximum i-Bus 2 cycle time duration in microseconds
#define IB2_TIMEOUT         100 // i-Bus 2 timeout, back to no signal state, expressed in microseconds

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char PacketSubtype:4;
	unsigned char SynchronizationLost:1;
	unsigned char FailsafeTriggered:1;
	unsigned char Length; // Packet length in bytes, Channels has a variable length
	unsigned char AddressLevel1:3;
	unsigned char AddressLevel2:3;
	unsigned char Reserved2:2;
	unsigned char Channels[SES_MAX_CHANNELS_LENGTH];
	unsigned char CRC8;
} sIB2_Channels;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGeneric;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char Reserved[1+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGetParam;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGetType;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandGetValue;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength;
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_CommandSetParam;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength;
	sIB2PA_ReceiverInternalSensors InternalSensors; 
	unsigned char Reserved[EB2_MAX_PARAM_LENGTH-sizeof(sIB2PA_ReceiverInternalSensors)];
	unsigned char CRC8;
} sIB2_CommandSetParamReceiverInternalSensors;

typedef union __attribute__((packed))
{
	sIB2_CommandGeneric Generic;
	sIB2_CommandGetType GetType;
	sIB2_CommandGetValue GetValue;
	sIB2_CommandGetParam GetParam;
	sIB2_CommandSetParam SetParam;
	sIB2_CommandSetParamReceiverInternalSensors SetParamReceiverInternalSensors;
} uIB2_Command;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Reserved[3+EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseGeneric;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength; // 0 if parameter not supported
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseGetParam;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	eIB_DeviceType Type;
	unsigned char ValueLength; // From 1 to 64
	uIB2_RequiredResources RequiredResources;
	unsigned char Reserved[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseGetType;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned char Value[IB_MAX_VALUE_LENGTH];
	unsigned char Reserved[3];
	unsigned char CRC8;
} sIB2_ResponseGetValue;

typedef struct __attribute__((packed))
{
	unsigned char PacketType:2;
	unsigned char CommandCode:6;
	unsigned short ParamType;
	unsigned char ParamLength; // 0 if parameter not supported
	unsigned char Reserved[EB2_MAX_PARAM_LENGTH];
	unsigned char CRC8;
} sIB2_ResponseSetParam;

typedef union __attribute__((packed))
{
	sIB2_ResponseGeneric Generic;
	sIB2_ResponseGetType GetType;
	sIB2_ResponseGetValue GetValue;
	sIB2_ResponseGetParam GetParam;
	sIB2_ResponseSetParam SetParam;
} uIB2_Response;

#endif // !defined(I_BUS_2_PRIVATE_H)
