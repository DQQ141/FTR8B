#if !defined(USART_NEW_PORT_H)
#define USART_NEW_PORT_H

#include "System definitions.h"
#include "USART.h"

typedef void tUSART_NPT_ReceiveCallback(unsigned long NbReceivedBytes);
typedef void tUSART_NPT_TransmitCallback(void);

extern const unsigned char USART_NPT_ReceiveTimeDelay;
extern const unsigned char USART_NPT_TransmitTimeDelay;
extern const unsigned char USART_NPT_TransmitCompleteTimeDelay;

void USART_NPT_Deinit(unsigned long PortNb);
unsigned long USART_NPT_GetNbReceivedBytes(unsigned long PortNb);
void USART_NPT_Init(unsigned long PortNb,const sUSART_Config *pUSARTConfig,BOOL IsIRQPriorityHigh);
void USART_NPT_ReceiveSetup(unsigned long PortNb,const void *pData,unsigned long DataLength,BOOL IsCircular,tUSART_NPT_ReceiveCallback *pCallback);
void USART_NPT_ReceiveStart(unsigned long PortNb);
void USART_NPT_ReceiveStop(unsigned long PortNb);
void USART_NPT_TransmitSetup(unsigned long PortNb,const void *pData,unsigned long DataLength,tUSART_NPT_TransmitCallback *pCallback);
void USART_NPT_TransmitStart(unsigned long PortNb);

#endif // !dedined(USART_NEW_PORT_H)
