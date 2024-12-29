#if !defined(I_BUS_2_DEVICE_LIBRARY_H)
#define I_BUS_2_DEVICE_LIBRARY_H

#include "System definitions.h"

#include "i-Bus.h"
#include "SES AFHDS3 external.h"
#include "USART.h"

typedef void tUSART_ReceiveCallback(unsigned long NbReceivedBytes);

typedef enum
{
	IB2ST_NO_SIGNAL,
	IB2ST_CHANNELS_RECEIVED,
	IB2ST_CONFIGURED
} eIB2D_Status;

typedef unsigned char tIB2D_GetParamCallback(unsigned short ParamType,unsigned char *pParamValue);
typedef unsigned char tIB2D_SetParamCallback(unsigned short ParamType,unsigned char ParamLength,const unsigned char *pParamValue);
typedef void tIB2D_GetValueCallback(void *pValue);
typedef uIB2_RequiredResources tIB2D_RequiredResourcesCallback(void);
typedef void tIB2D_SetChannelsCallback(eSES_SignalStatus SignalStatus);
typedef void tIB2D_SetChannelsTypeCallback(void);
typedef void tIB2D_SetFailsafeCallback(unsigned long CallbackValue);
typedef void tIB2D_StatusChangedCallback(eIB2D_Status Status);

typedef void tIB2DTIM_Deinit(void);
typedef void tIB2DTIM_Init(void);
typedef void tIB2DTIM_SetNextIRQTime(unsigned long Time);
typedef void tIB2DTIM_StartTimer(unsigned long FirstTime);
typedef void tIB2DTIM_StopTimer(void);

typedef void tIB2DUSART_Deinit(void);
typedef void tIB2DUSART_Init(const sUSART_Config *pUSARTConfig);
typedef void tIB2DUSART_ReceiveSetup(const void *pData,unsigned long DataLength,tUSART_ReceiveCallback *pCallback);
typedef void tIB2DUSART_ReceiveStart(void);
typedef void tIB2DUSART_ReceiveStop(void);
typedef void tIB2DUSART_TransmitSetup(const void *pData,unsigned long DataLength);
typedef void tIB2DUSART_TransmitStart(void);

typedef struct
{
	tIB2D_GetParamCallback *pIB2D_GetParamCallback;
	tIB2D_SetParamCallback *pIB2D_SetParamCallback;
	tIB2D_GetValueCallback *pIB2D_GetValueCallback;
	tIB2D_RequiredResourcesCallback *pIB2D_RequiredResourcesCallback;
	tIB2D_SetChannelsCallback *pIB2D_SetChannelsCallback;
	tIB2D_SetChannelsTypeCallback *pIB2D_SetChannelsTypeCallback;
	tIB2D_SetFailsafeCallback *pIB2D_SetFailsafeCallback;
	tIB2DTIM_Deinit *pIB2DTIM_Deinit;
	tIB2DTIM_Init *pIB2DTIM_Init;
	tIB2D_StatusChangedCallback *pIB2D_StatusChangedCallback;
	tIB2DTIM_SetNextIRQTime *pIB2DTIM_SetNextIRQTime;
	tIB2DTIM_StartTimer *pIB2DTIM_StartTimer;
	tIB2DTIM_StopTimer *pIB2DTIM_StopTimer;
	tIB2DUSART_Deinit *pIB2DUSART_Deinit;
	tIB2DUSART_Init *pIB2DUSART_Init;
	tIB2DUSART_ReceiveSetup *pIB2DUSART_ReceiveSetup;
	tIB2DUSART_ReceiveStart *pIB2DUSART_ReceiveStart;
	tIB2DUSART_ReceiveStop *pIB2DUSART_ReceiveStop;
	tIB2DUSART_TransmitSetup *pIB2DUSART_TransmitSetup;
	tIB2DUSART_TransmitStart *pIB2DUSART_TransmitStart;
	unsigned char *pChannelsPacked;
	unsigned char *pChannelsType; // 5 bits per channel from the values defined in SES_CT_*
	unsigned char *pFailsafePacked;
	eIB_DeviceType SensorType;
	unsigned char SensorValueLength;
	unsigned char USARTReceiveTimeDelay;
	unsigned char USARTReceiveTimeoutOffset;
	unsigned char USARTTransmitTimeDelay;
	unsigned char Padding[3];
} sIB2D_InitStruct;

extern volatile unsigned long SYS_SysTickMs;

void IB2D_MsIRQHandler(void);
void IB2D_Deinit(void);
void IB2D_Init(const sIB2D_InitStruct *pInitStruct);

void IB2DTIM_TimerIRQHandler(void);

void SES_UnpackChannels(const void *pPackedChannels,signed short *pChannels,const unsigned char *pChannelsType);

#endif // !defined(I_BUS_2_DEVICE_LIBRARY_H)
