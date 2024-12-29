#if !defined(I_BUS_1_DEVICE_LIBRARY_H)
#define I_BUS_1_DEVICE_LIBRARY_H

#include "i-Bus.h"

#define IB1D_USART_SPEED          115200
#define IB1D_USART_RX_BUFFER_SIZE 64

#define IB1D_NB_MAX_DEVICES 5

typedef void tUSART_ReceiveCallback(unsigned long NbReceivedBytes);

typedef enum
{
	IBSBS_DISABLED,
	IBSBS_OUT,
	IBSBS_IN,
} sIB1D_BufferState;

typedef struct __attribute__((packed))
{
	eIB_DeviceType DeviceType;
	unsigned char DeviceValueLength;
	BOOL IsSensor;
} sIB1D_Description;

typedef enum
{
	IB1ST_NO_SIGNAL,
	IB1ST_CONFIGURING,
	IB1ST_CONFIGURED,
	IB1ST_WRONG_PORT,
	IB1ST_NO_STATUS // Used only by IB1D_PreviousStatus to force status update
} eIB1D_Status;

typedef void tIB1DTIM_TimerCompareIRQHandler(void);

extern const sIB1D_Description IB1D_Description[];
extern const unsigned long IB1D_ReceiveTimeoutOffset;
extern const unsigned long IB1D_TransmitTimeDelay;
extern const unsigned char IB1D_NbDevices;

// Functions provided by the i-Bus 1 device library
void IB1D_MsIRQHandler(void);
void IB1D_Init(void);

void IB1D_GetValueCallback(unsigned long DeviceNb,void *pValue,unsigned long BlockNb);
unsigned long IB1D_GetParamCallback(unsigned long DeviceNb,unsigned short ParameterType,void *pParameterValue);
void IB1D_ServoOutCallback(const unsigned short *pChannels);
void IB1D_SetBufferState(sIB1D_BufferState BufferState);
BOOL IB1D_SetParamCallback(unsigned long DeviceNb,BOOL IsGlobal,unsigned short ParamType,unsigned long ParamLength,const void *pParamValue);
void IB1D_StatusChangedCallback(eIB1D_Status Status);

void IB1DTIM_TimerIRQHandler(void);
void IB1DTIM_SetNextIRQTime(unsigned long Time);
void IB1DTIM_StartTimer(unsigned long FirstTime);
void IB1DTIM_StopTimer(void);

void IB1DUSART_ReceiveSetup(const void *pData,unsigned long DataLength,tUSART_ReceiveCallback *pCallback);
void IB1DUSART_ReceiveStart(void);
void IB1DUSART_TransmitSetup(const void *pData,unsigned long DataLength);
void IB1DUSART_TransmitStart(void);

#endif // !defined(I_BUS_1_DEVICE_LIBRARY_H)
