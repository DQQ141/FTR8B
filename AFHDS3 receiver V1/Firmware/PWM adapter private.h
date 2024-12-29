#if !defined(PWM_ADAPTER_PRIVATE_H)
#define PWM_ADAPTER_PRIVATE_H

#include "i-Bus 2 device library.h"
#include "SES AFHDS3 external.h"
#include "USART.h"

static unsigned char IB2D_GetParamCallback(unsigned short ParamType,unsigned char *pParamValue);
static void IB2D_GetValueCallback(void *pValue);
static uIB2_RequiredResources IB2D_RequiredResourcesCallback(void);
static void IB2D_SetChannelsCallback(eSES_SignalStatus SignalStatus);
static void IB2D_SetChannelsTypeCallback(void);
static void IB2D_SetFailsafeCallback(unsigned long CallbackValue);
static void IB2D_SetFailsafeUnpack(void);
static void IB2D_StatusChangedCallback(eIB2D_Status Status);
static unsigned char IB2D_SetParamCallback(unsigned short ParamType,unsigned char ParamLength,const unsigned char *pParamValue);
static void IB2DTIM_Deinit(void);
static void IB2DTIM_Init(void);
static void IB2DTIM_SetNextIRQTime(unsigned long Time);
static void IB2DTIM_StartTimer(unsigned long FirstTime);
static void IB2DTIM_StopTimer(void);
static void IB2DUSART_Deinit(void);
static void IB2DUSART_Init(const sUSART_Config *pUSARTConfig);
static void IB2DUSART_ReceiveSetup(const void *pData,unsigned long DataLength,tUSART_ReceiveCallback *pCallback);
static void IB2DUSART_ReceiveStart(void);
static void IB2DUSART_ReceiveStop(void);
static void IB2DUSART_TransmitSetup(const void *pData,unsigned long DataLength);
static void IB2DUSART_TransmitStart(void);

#endif // !defined(PWM_ADAPTER_PRIVATE_H)
