#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#endif
#include <string.h>

#include "System definitions.h"

#include "i-Bus.h"
#include "i-Bus 1.h"
#include "i-Bus 2.h"
#include "LED.h"
#include "Main.h"
#include "NPT timer.h"
#include "PWM adapter.h"
#include "SES AFHDS3 external.h"
#include "USART New Port.h"

#include "PWM adapter private.h"
#include "System receiver.h"

#define APP_ConfigPWM APP_Config.C.PWM

static unsigned char PWMA_ChannelsType[SES_CHANNELS_TYPE_LENGTH];

static uIB2_RequiredResources PWMA_RequiredResources;

// New Port A is used as i-Bus input
#define PWMA_I_BUS_NEW_PORT_NB 0
#define PWMA_I_BUS_CHANNEL_NB (NPT_NB_CHANNELS-1)
static const eSES_NewPortType PWM_NewPortTypes[SES_NPT_NB_MAX_PORTS]={SES_NPT_NONE,SES_NPT_PWM,SES_NPT_PWM,SES_NPT_PWM};

const unsigned char IB1D_NbDevices=1;
const sIB1D_Description IB1D_Description[1]={
	{
		.DeviceType=(eIB_DeviceType)(IBDT_HUB_1_PORTS_PWM+NPT_NB_CHANNELS-2),
		.DeviceValueLength=2,
		.IsSensor=FALSE
	}
};
static const sIB2D_InitStruct IB2D_InitStruct={
	.pIB2D_GetParamCallback=IB2D_GetParamCallback,
	.pIB2D_SetParamCallback=IB2D_SetParamCallback,
	.pIB2D_GetValueCallback=IB2D_GetValueCallback,
	.pIB2D_RequiredResourcesCallback=IB2D_RequiredResourcesCallback,
	.pIB2D_SetChannelsCallback=IB2D_SetChannelsCallback,
	.pIB2D_SetChannelsTypeCallback=IB2D_SetChannelsTypeCallback,
	.pIB2D_SetFailsafeCallback=IB2D_SetFailsafeCallback,
	.pIB2D_StatusChangedCallback=IB2D_StatusChangedCallback,
	.pIB2DTIM_Deinit=IB2DTIM_Deinit,
	.pIB2DTIM_Init=IB2DTIM_Init,
	.pIB2DTIM_SetNextIRQTime=IB2DTIM_SetNextIRQTime,
	.pIB2DTIM_StartTimer=IB2DTIM_StartTimer,
	.pIB2DTIM_StopTimer=IB2DTIM_StopTimer,
	.pIB2DUSART_Deinit=IB2DUSART_Deinit,
	.pIB2DUSART_Init=IB2DUSART_Init,
	.pIB2DUSART_ReceiveSetup=IB2DUSART_ReceiveSetup,
	.pIB2DUSART_ReceiveStart=IB2DUSART_ReceiveStart,
	.pIB2DUSART_ReceiveStop=IB2DUSART_ReceiveStop,
	.pIB2DUSART_TransmitSetup=IB2DUSART_TransmitSetup,
	.pIB2DUSART_TransmitStart=IB2DUSART_TransmitStart,
	.pChannelsPacked=APP_ChannelsPacked1,
	.pChannelsType=PWMA_ChannelsType,
	.pFailsafePacked=APP_UsedFailsafePacked,
	.SensorType=(eIB_DeviceType)(IBDT_HUB_1_PORTS_PWM+NPT_NB_CHANNELS-2),
	.SensorValueLength=2,
	.USARTReceiveTimeDelay=9,
	.USARTReceiveTimeoutOffset=10,
	.USARTTransmitTimeDelay=9
};

const unsigned long IB1D_ReceiveTimeoutOffset=10;
const unsigned long IB1D_TransmitTimeDelay=9;

#pragma clang diagnostic ignored "-Wunused-parameter"
void PWMA_IBus1TimerIRQHandler(unsigned long ChannelNb,unsigned char AreIRQPrioritiesHigh)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	IB1DTIM_TimerIRQHandler();
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void PWMA_IBus2TimerIRQHandler(unsigned long ChannelNb,unsigned char AreIRQPrioritiesHigh)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	IB2DTIM_TimerIRQHandler();
	return;
}

// No supported parameter to read
#pragma clang diagnostic ignored "-Wunused-parameter"
unsigned long IB1D_GetParamCallback(unsigned long DeviceNb,unsigned short ParameterType,void *pParameterValue)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	return 0;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void IB1D_GetValueCallback(unsigned long DeviceNb,void *pValue,unsigned long BlockNb)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	SYS_STORE_USHORT(pValue,DIV10(ADC_GetInternalVoltage()))
	return;
}

void IB1D_ServoOutCallback(const unsigned short *pChannels)
{
	unsigned long PWMChannelNb;

	for (PWMChannelNb=0;PWMChannelNb<NPT_NB_CHANNELS-1;PWMChannelNb++)
	{
		unsigned long ChannelNb;
		unsigned long ChannelValueUs;

		ChannelNb=APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMChannelsNb[PWMChannelNb];
		if (ChannelNb<IB1_NB_CHANNELS)
			ChannelValueUs=SYS_LoadUShort(&pChannels[ChannelNb])&0xFFF;
		else
		{
			const unsigned char *pHighByte;

			pHighByte=(const unsigned char *)pChannels+(ChannelNb-IB1_NB_CHANNELS)*3*2+1;
			ChannelValueUs=
				(((const unsigned long)*(pHighByte+0)&0xF0)>>4) |
				(((const unsigned long)*(pHighByte+2)&0xF0)   ) |
				(((const unsigned long)*(pHighByte+4)&0xF0)<<4);
		}
		if (ChannelValueUs<IB1_CHANNEL_US_VALUE_MIN)
			ChannelValueUs=IB1_CHANNEL_US_VALUE_MIN;
		else if (ChannelValueUs>IB1_CHANNEL_US_VALUE_MAX)
			ChannelValueUs=IB1_CHANNEL_US_VALUE_MAX;
		APP_Channels1[PWMChannelNb]=(((signed long)ChannelValueUs-IB1_CHANNEL_US_VALUE_MID)*2147484)>>16;
	}
	NPT_SetChannels();
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void IB1D_SetBufferState(sIB1D_BufferState BufferState)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
BOOL IB1D_SetParamCallback(unsigned long DeviceNb,BOOL IsGlobal,unsigned short ParamType,unsigned long ParamLength,const void *pParamValue)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	return 0;
}

void IB1D_StatusChangedCallback(eIB1D_Status Status)
{
	const sLED_Pattern *pPattern;

	switch (Status)
	{
	case IB1ST_NO_STATUS:
	case IB1ST_NO_SIGNAL:
	case IB1ST_WRONG_PORT:
		pPattern=&LED_PWMA_NoSignal;
		PWMA_RequiredResources.All=0;
		PWMA_RequiredResources.Types.ChannelsType=TRUE;
		PWMA_RequiredResources.Types.Failsafe=TRUE;
		break;
	case IB1ST_CONFIGURING:
		pPattern=&LED_Bind;
		break;
	case IB1ST_CONFIGURED:
		pPattern=&LED_PWMA_Configured;
		break;
	}
	LED_SetPattern(pPattern);
	return;
}

void IB1DTIM_SetNextIRQTime(unsigned long Time)
{
	NPTTIM_SetNextCompareIRQTime(PWMA_I_BUS_CHANNEL_NB,Time*4,FALSE);
	return;
}

void IB1DTIM_StartTimer(unsigned long FirstTime)
{
	NPTTIM_SetNextCompareIRQTimeFromNow(PWMA_I_BUS_CHANNEL_NB,FALSE,FirstTime*4,FALSE);
	return;
}

void IB1DTIM_StopTimer(void)
{
	NPTTIM_FreezeCompareIRQ(PWMA_I_BUS_CHANNEL_NB);
	return;
}

void IB1DUSART_ReceiveSetup(const void *pData,unsigned long DataLength,tUSART_ReceiveCallback *pCallback)
{
	USART_NPT_ReceiveSetup(PWMA_I_BUS_NEW_PORT_NB,pData,DataLength,FALSE,pCallback);
	return;
}

void IB1DUSART_ReceiveStart(void)
{
	USART_NPT_ReceiveStart(PWMA_I_BUS_NEW_PORT_NB);
	return;
}

void IB1DUSART_TransmitSetup(const void *pData,unsigned long DataLength)
{
	USART_NPT_TransmitSetup(PWMA_I_BUS_NEW_PORT_NB,pData,DataLength,NULL);
	return;
}

void IB1DUSART_TransmitStart(void)
{
	USART_NPT_TransmitStart(PWMA_I_BUS_NEW_PORT_NB);
	return;
}

static unsigned char IB2D_GetParamCallback(unsigned short ParamType,unsigned char *pParamValue)
{
	// Get the assigned RC channel to a given port
	if (ParamType>=IB2PC_PWM_ADAPTER_CHANNEL &&
		ParamType<IB2PC_PWM_ADAPTER_CHANNEL+NPT_NB_CHANNELS-1)
	{
		sIB2PA_PWMAdapterChannel *pChannel;
		unsigned long PortNb;

		PortNb=ParamType-IB2PC_PWM_ADAPTER_CHANNEL;
		pChannel=(sIB2PA_PWMAdapterChannel *)pParamValue;
		pChannel->ChannelNb=APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMChannelsNb[PortNb];
		return sizeof(sIB2PA_PWMAdapterChannel);
	}
	// Get the frequency of a given port
	else if (ParamType>=IB2PC_PWM_ADAPTER_FREQUENCY &&
		ParamType<IB2PC_PWM_ADAPTER_FREQUENCY+NPT_NB_CHANNELS-1)
	{
		sIB2PA_PWMAdapterFrequency *pFrequency;
		unsigned long PortNb;

		PortNb=ParamType-IB2PC_PWM_ADAPTER_FREQUENCY;
		pFrequency=(sIB2PA_PWMAdapterFrequency *)pParamValue;
		pFrequency->Frequency=APP_PWMFrequenciesV1.PWMFrequencies[PortNb];
		pFrequency->Synchronized=(APP_PWMFrequenciesV1.Synchronized>>PortNb) & 1;
		return sizeof(sIB2PA_PWMAdapterFrequency);
	}
	else
		return 0;
}

static void IB2D_GetValueCallback(void *pValue)
{
	SYS_STORE_USHORT(pValue,DIV10(ADC_GetInternalVoltage()))
	return;
}

static uIB2_RequiredResources IB2D_RequiredResourcesCallback(void)
{
	return PWMA_RequiredResources;
}

static void IB2D_SetChannelsCallback(eSES_SignalStatus SignalStatus)
{
	APP_SignalStatus=SignalStatus;
	if (PWMA_RequiredResources.All!=0)
		return;
	SES_UnpackChannels(APP_ChannelsPacked1,APP_Channels2,PWMA_ChannelsType);
	{
		unsigned long PWMChannelNb;

		for (PWMChannelNb=0;PWMChannelNb<NPT_NB_CHANNELS-1;PWMChannelNb++)
		{
			APP_Channels1[PWMChannelNb]=
				APP_Channels2[APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMChannelsNb[PWMChannelNb]];
		}
	}
	NPT_SetChannels();
	return;
}

static void IB2D_SetChannelsTypeCallback(void)
{
	PWMA_RequiredResources.Types.ChannelsType=FALSE;
	IB2D_SetFailsafeUnpack();
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
static void IB2D_SetFailsafeCallback(unsigned long CallbackValue)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	PWMA_RequiredResources.Types.Failsafe=FALSE;
	IB2D_SetFailsafeUnpack();
	return;
}

static void IB2D_SetFailsafeUnpack(void)
{
	signed short Failsafe[SES_NB_MAX_CHANNELS];
	unsigned long PWMChannelNb;

	if (PWMA_RequiredResources.Types.Failsafe ||
		PWMA_RequiredResources.Types.ChannelsType)
	{
		return;
	}
	SES_UnpackChannels(APP_UsedFailsafePacked,Failsafe,PWMA_ChannelsType);
	for (PWMChannelNb=0;PWMChannelNb<NPT_NB_CHANNELS-1;PWMChannelNb++)
	{
		APP_UsedFailsafe[PWMChannelNb]=
			Failsafe[APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMChannelsNb[PWMChannelNb]];
	}
	return;
}

static unsigned char IB2D_SetParamCallback(unsigned short ParamType,unsigned char ParamLength,const unsigned char *pParamValue)
{
	// Assign a RC channer to a given port
	if (ParamType>=IB2PC_PWM_ADAPTER_CHANNEL &&
		ParamType<IB2PC_PWM_ADAPTER_CHANNEL+NPT_NB_CHANNELS-1)
	{
		const sIB2PA_PWMAdapterChannel *pChannel;
		unsigned long PortNb;

		PortNb=ParamType-IB2PC_PWM_ADAPTER_CHANNEL;
		pChannel=(const sIB2PA_PWMAdapterChannel *)pParamValue;
		if (ParamLength!=sizeof(sIB2PA_PWMAdapterChannel) ||
			pChannel->ChannelNb>=SES_NB_MAX_CHANNELS)
		{
			return 0;
		}
		if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMChannelsNb[PortNb]!=pChannel->ChannelNb)
		{
			APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMChannelsNb[PortNb]=pChannel->ChannelNb;
			IB2D_SetFailsafeUnpack();
			APP_ConfigChanged=TRUE;
		}
		return ParamLength;
	}
	// Assign a frequency to a given port
	else if (ParamType>=IB2PC_PWM_ADAPTER_FREQUENCY &&
		ParamType<IB2PC_PWM_ADAPTER_FREQUENCY+NPT_NB_CHANNELS-1)
	{
		const sIB2PA_PWMAdapterFrequency *pFrequency;
		unsigned long PortNb;

		PortNb=ParamType-IB2PC_PWM_ADAPTER_FREQUENCY;
		pFrequency=(const sIB2PA_PWMAdapterFrequency *)pParamValue;
		if (ParamLength!=sizeof(sIB2PA_PWMAdapterFrequency) ||
			pFrequency->Frequency==0 ||
			(pFrequency->Frequency>SES_NB_NARROW_PWM_STANDARDS && pFrequency->Frequency<SES_PWM_FREQUENCY_MIN) ||
			pFrequency->Frequency>SES_PWM_FREQUENCY_MAX)
		{
			return 0;
		}
		if (APP_PWMFrequenciesV1.PWMFrequencies[PortNb]!=pFrequency->Frequency ||
			((APP_PWMFrequenciesV1.Synchronized & (1<<PortNb))==0)!=(pFrequency->Synchronized==0))
		{
			APP_PWMFrequenciesV1.PWMFrequencies[PortNb]=pFrequency->Frequency;
			APP_PWMFrequenciesV1.Synchronized&=~(1<<PortNb);
			if (pFrequency->Synchronized)
				APP_PWMFrequenciesV1.Synchronized|=1<<PortNb;
			SYS_QueueLowPriorityIRQCallback(PWMA_CallPWMSetFrequency,0);
			APP_ConfigChanged=TRUE;
		}
		return ParamLength;
	}
	else
		return 0;
}

static void IB2D_StatusChangedCallback(eIB2D_Status Status)
{
	const sLED_Pattern *pPattern;

	switch (Status)
	{
	case IB2ST_NO_SIGNAL:
		pPattern=&LED_PWMA_NoSignal;
		PWMA_RequiredResources.All=0;
		PWMA_RequiredResources.Types.ChannelsType=TRUE;
		PWMA_RequiredResources.Types.Failsafe=TRUE;
		APP_SignalStatus=SES_SS_FAILSAFE;
		break;
	case IB2ST_CHANNELS_RECEIVED:
		pPattern=&LED_Bind;
		break;
	case IB2ST_CONFIGURED:
		pPattern=&LED_PWMA_Configured;
		break;
	}
	LED_SetPattern(pPattern);
	return;
}

// Unused
static void IB2DTIM_Deinit(void)
{
	return;
}

static void IB2DTIM_Init(void)
{
	NPTTIM_EnableChannel(PWMA_I_BUS_CHANNEL_NB,FALSE,PWMA_IBus2TimerIRQHandler,TRUE,TRUE);
	return;
}

static void IB2DTIM_SetNextIRQTime(unsigned long Time)
{
	NPTTIM_SetNextCompareIRQTime(PWMA_I_BUS_CHANNEL_NB,Time*4,FALSE);
	return;
}

static void IB2DTIM_StartTimer(unsigned long FirstTime)
{
	NPTTIM_SetNextCompareIRQTimeFromNow(PWMA_I_BUS_CHANNEL_NB,FALSE,FirstTime*4,FALSE);
	return;
}

static void IB2DTIM_StopTimer(void)
{
	NPTTIM_FreezeCompareIRQ(PWMA_I_BUS_CHANNEL_NB);
	return;
}

// Unused
static void IB2DUSART_Deinit(void)
{
	return;
}

static void IB2DUSART_Init(const sUSART_Config *pUSARTConfig)
{
	USART_NPT_Init(PWMA_I_BUS_NEW_PORT_NB,pUSARTConfig,TRUE);
	return;
}

static void IB2DUSART_ReceiveSetup(const void *pData,unsigned long DataLength,tUSART_ReceiveCallback *pCallback)
{
	USART_NPT_ReceiveSetup(PWMA_I_BUS_NEW_PORT_NB,pData,DataLength,FALSE,pCallback);
	return;
}

static void IB2DUSART_ReceiveStart(void)
{
	USART_NPT_ReceiveStart(PWMA_I_BUS_NEW_PORT_NB);
	return;
}

static void IB2DUSART_ReceiveStop(void)
{
	USART_NPT_ReceiveStop(PWMA_I_BUS_NEW_PORT_NB);
	return;
}

static void IB2DUSART_TransmitSetup(const void *pData,unsigned long DataLength)
{
	USART_NPT_TransmitSetup(PWMA_I_BUS_NEW_PORT_NB,pData,DataLength,NULL);
	return;
}

static void IB2DUSART_TransmitStart(void)
{
	USART_NPT_TransmitStart(PWMA_I_BUS_NEW_PORT_NB);
	return;
}

__attribute((noreturn)) void PWMA_Activate(eLNK_ReceiverRole ReceiverRole)
{
	APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole=ReceiverRole;
	memcpy(APP_Config.NewPortTypes,PWM_NewPortTypes,sizeof(APP_Config.NewPortTypes));
	APP_SaveConfig();
#if defined(PN_FBR12) 	
    RAM_BindSignature1=APP_BIND_SIGNATURE_1;
    RAM_BindSignature2=APP_BIND_SIGNATURE_2;
#endif
	NVIC_SystemReset();

}

#pragma clang diagnostic ignored "-Wunused-parameter"
void PWMA_CallPWMSetFrequency(unsigned long CallbackValue)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	NPT_SetPWMFrequencies();
	return;
}

void PWMA_Init(void)
{
	LED_SetPattern(&LED_PWMA_NoSignal);
	APP_SignalStatus=SES_SS_OUT_OF_SYNC;
	if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER)
	{
		NPTTIM_EnableChannel(PWMA_I_BUS_CHANNEL_NB,FALSE,PWMA_IBus1TimerIRQHandler,TRUE,TRUE);
		USART_NPT_Init(PWMA_I_BUS_NEW_PORT_NB,&IB1_USARTConfig,TRUE);
		IB1D_Init();
	}
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
	{
		PWMA_RequiredResources.All=0;
		PWMA_RequiredResources.Types.ChannelsType=TRUE;
		PWMA_RequiredResources.Types.Failsafe=TRUE;
		IB2D_Init(&IB2D_InitStruct);
	}
	return;
}
