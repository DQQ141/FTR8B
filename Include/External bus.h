#if !defined(EXTERNAL_BUS_H)
#define EXTERNAL_BUS_H

#include "System definitions.h"

#include "AFHDS3 queue medium priority IRQ library.h"
#include "i-Bus 2.h"
#include "SES AFHDS3 external.h"
#include "USART.h"

typedef enum
{
	EB_BT_IBUS1,
	EB_BT_IBUS2,
	EB_BT_SBUS1
} eEB_BusType;

typedef enum __attribute__((packed))
{
	EB_SBPT_OUTPUT, // i-Bus, i-Bus 2 or S-bus servos output
	EB_SBPT_INPUT  // i-bus sensors input
} eEB_SingleBusPortType;

#if AFHDS3_VERSION==0
typedef struct
{
	unsigned char ID;
	unsigned char Padding;
	unsigned short ParamType;
} sIB1_GetParam;

typedef struct
{
	unsigned char ID;
	unsigned char ParamLength;
	unsigned short ParamType;
	union
	{
		unsigned char UChar;
		signed char SChar;
		unsigned short UShort;
		signed short SShort;
		unsigned long ULong;
		signed long SLong;
	} ParamValue;
} sIB1_SetParam;

typedef struct
{
	uIB2_ID ID;
	unsigned char Padding;
	unsigned short ParamType;
} sIB2_GetParam;

typedef struct
{
	uIB2_ID ID;
	unsigned char ParamLength;
	unsigned short ParamType;
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH];
} sIB2_SetParam;
#endif

typedef struct
{
	const unsigned char *pChannelsType;
	const unsigned char *pFailsafePacked;
	const unsigned char *pSupportedSensors;
	unsigned char RXPayloadLength;
	eEB_BusType BusType;
#if defined(EB_SINGLE_PORT)
	eEB_SingleBusPortType SingleBusPortType;
#endif
	BOOL IsFailsafeInNoOutputMode;
	BOOL IsOneWay;
#if defined(EB_SINGLE_PORT)
	unsigned char Padding[3];
#endif
} sEB_Init;

#if AFHDS3_VERSION==0
	extern unsigned char APP_ChannelsPacked[SES_MAX_CHANNELS_LENGTH];
	extern signed short APP_Channels[SES_NB_MAX_CHANNELS];
	extern unsigned short APP_ChannelsUs[SES_NB_MAX_CHANNELS];
	extern eSES_SignalStatus APP_SignalStatus;
	extern const sUSART_Config EB_IBus1USARTConfig;
	extern const sUSART_Config EB_IBus2USARTConfig;
	extern volatile unsigned long SYS_SysTickMs;
#endif

void EBTIM_TimerCallback(void);

#if AFHDS3_VERSION==0
	BOOL IB1_GetParam(sIB1_GetParam *pGetParam);
	BOOL IB1_SetParam(sIB1_SetParam *pSetParam);
	BOOL IB1_SetupServo(unsigned char ChannelNb);
	BOOL IB2_GetParam(sIB2_GetParam *pGetParam);
	BOOL IB2_SetParam(sIB2_SetParam *pSetParam);
#endif
BOOL EB_BuildRealtimePacketCallback(void *pPayload);
void EB_FailsafeChanged(void);
void EB_Init(const sEB_Init *pInit);
void EB_SetChannels(void);

void IB1_GetParamResponse(unsigned char ID,unsigned short ParamType,unsigned long ParamLength,const void *pParamValue);
void IB1_SetParamResponse(unsigned char ID,unsigned short ParamType);
void IB1_SetupServoResponse(unsigned char ChannelNb,unsigned char ID,unsigned char OutputNb);
void IB2_GetParamResponse(const sSES_CA_IBus2GetParamResponse *pResponse,unsigned long ParamLength);
void IB2_SetParamResponse(const sSES_CA_IBus2SetParamResponse *pResponse);

unsigned long ADC_GetInternalVoltage(void);

unsigned short LNKTIM_GetCounter(void);

unsigned short SYS_CalcChecksum16(const void *pData,unsigned long DataLength);
void SYS_QueueMediumPriorityIRQCallback(tSYS_MediumPriorityIRQCallback *pCallback,unsigned long CallbackValue);

#endif // !defined(EXTERNAL_BUS_H)
