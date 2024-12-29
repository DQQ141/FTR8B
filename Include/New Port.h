#if !defined(NEW_PORT_H)
#define NEW_PORT_H

#include "System definitions.h"

#include "AFHDS3 queue low priority IRQ library.h"
#include "AFHDS3 queue medium priority IRQ library.h"

#include "i-Bus.h"
#include "i-Bus 2.h"
#include "SES AFHDS3 external.h"
#include "USART.h"

#define NPT_CHANNEL_US_VALUE_MIN  750
#define NPT_CHANNEL_US_VALUE_MID 1500
#define NPT_CHANNEL_US_VALUE_MAX 2250

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

typedef struct
{
	const unsigned char *pChannelsType;
	const sSES_PWMFrequenciesAPPV1 *pPWMFrequenciesAPP;
	const unsigned char *pChannelsPacked;
	const signed short *pChannels;
	const unsigned char *pRXFirstPWMRCChannelNb;
	const unsigned char *pFailsafePacked;
	const signed short *pFailsafe;
	const BOOL *pIsFailsafeInNoOutputMode;
	const eSES_SignalStatus *pSignalStatus;
	const unsigned char *pSupportedSensors;
	const sUSART_Config *pWSUSARTConfig;
	void *pWSTXBuffer;
	void *pWSRXBuffer;
	unsigned long WSTXBufferSize;
	unsigned long WSRXBufferSize;
	const eSES_NewPortType *pNewPortTypes;
	unsigned char RXPayloadLength;
	unsigned char NbInternalSensors;
	unsigned char Padding[2];
} sNPT_Init;

typedef struct __attribute__((packed))
{
	unsigned char ValueLength;
	eIB_DeviceType Type;
	unsigned char Address;
	unsigned char Value[IB_MAX_VALUE_LENGTH];
} sNPT_Sensor;

extern const sUSART_Config IB1_USARTConfig;
extern const sUSART_Config IB2_USARTConfig;

extern BOOL NPT_ChannelsSet;
extern volatile unsigned long SYS_SysTickMs;

extern const sNPT_Init *pNPT_InitStruct;
extern unsigned short NPT_ChannelsUs[SES_NB_MAX_CHANNELS]; // In 0.25us units
extern volatile BOOL NPT_DeinitializationInProgress;

BOOL IB1_GetParam(sIB1_GetParam *pGetParam);
BOOL IB1_SetParam(sIB1_SetParam *pSetParam);
BOOL IB1_SetupServo(unsigned char ChannelNb);
BOOL IB2_GetParam(sIB2_GetParam *pGetParam);
BOOL IB2_SetParam(sIB2_SetParam *pSetParam);

void NPT_MsIRQHandler(void);
BOOL NPT_BuildRealtimePacketCallback(void *pPayload,BOOL InternalSensorsOnly);
void NPT_FailsafeChanged(void);
void NPT_Init(const sNPT_Init *pInit);
void NPT_InitPort(unsigned long PortNb,eSES_NewPortType PortType);
void NPT_SetChannels(void);
void NPT_SetPWMFrequencies(void);
void NPT_SetWSUSARTConfig(void);
void NPT_StartDeinit(void);
BOOL NPT_WSDataReceivedCallback(const void *pData,unsigned long DataLength);

void IB1_GetParamResponse(unsigned char ID,unsigned short ParamType,unsigned long ParamLength,const void *pParamValue);
void IB1_SetParamResponse(unsigned char ID,unsigned short ParamType);
void IB1_SetupServoResponse(unsigned char ChannelNb,unsigned char ID,unsigned char OutputNb);
void IB2_GetParamResponse(const sSES_CA_IBus2GetParamResponse *pResponse,unsigned long ParamLength);
void IB2_SetParamResponse(const sSES_CA_IBus2SetParamResponse *pResponse);
BOOL NPT_GetInternalSensor(sNPT_Sensor *pSensor);

unsigned short LNKTIM_GetCounter(void);

unsigned short SYS_CalcChecksum16(const void *pData,unsigned long DataLength);
void SYS_QueueLowPriorityIRQCallback(tSYS_LowPriorityIRQCallback *pCallback,unsigned long CallbackValue);
void SYS_QueueLowPriorityIRQCallbackLast(tSYS_LowPriorityIRQCallback *pCallback,unsigned long CallbackValue);
void SYS_QueueMediumPriorityIRQCallback(tSYS_MediumPriorityIRQCallback *pCallback,unsigned long CallbackValue);

#endif // !defined(NEW_PORT_H)
