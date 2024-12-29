#if !defined(PWM_ADAPTER_H)
#define PWM_ADAPTER_H

#include "System definitions.h"

#include "LNK AFHDS3.h"

#include "i-Bus 1 device library.h"

void PWMA_IBus1TimerIRQHandler(unsigned long ChannelNb,unsigned char AreIRQPrioritiesHigh);
void PWMA_IBus2TimerIRQHandler(unsigned long ChannelNb,unsigned char AreIRQPrioritiesHigh);
unsigned long IB1D_GetParamCallback(unsigned long DeviceNb,unsigned short ParameterType,void *pParameterValue);
void IB1D_GetValueCallback(unsigned long DeviceNb,void *pValue,unsigned long BlockNb);
void IB1D_SetBufferState(sIB1D_BufferState BufferState);
BOOL IB1D_SetParamCallback(unsigned long DeviceNb,BOOL IsGlobal,unsigned short ParamType,unsigned long ParamLength,const void *pParamValue);
void IB1D_StatusChangedCallback(eIB1D_Status Status);
void IB1DUSART_ReceiveSetup(const void *pData,unsigned long DataLength,tUSART_ReceiveCallback *pCallback);
void IB1DUSART_ReceiveStart(void);
void IB1DUSART_TransmitSetup(const void *pData,unsigned long DataLength);
void IB1DUSART_TransmitStart(void);
void IB1DTIM_SetNextIRQTime(unsigned long Time);
void IB1DTIM_StartTimer(unsigned long FirstTime);
void IB1DTIM_StopTimer(void);
void PWMA_Activate(eLNK_ReceiverRole ReceiverRole);
void PWMA_CallPWMSetFrequency(unsigned long CallbackValue);
void PWMA_Init(void);

#endif // !defined(PWM_ADAPTER_H)
