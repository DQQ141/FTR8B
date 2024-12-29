#if !defined(USART_NEW_PORT_PRIVATE_H)
#define USART_NEW_PORT_PRIVATE_H

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#endif

#include "System definitions.h"

#include "USART New Port.h"

typedef struct
{
	USART_TypeDef *pRegs;
	DMA_Channel_TypeDef *pDMAChannel;
	IRQn_Type IRQNb;
	unsigned char DMAMUXChannelNb;
	unsigned char DMAMUXTXInputNb;
	unsigned char DMAMUXRXInputNb;
	unsigned long ReceiveDataMaxLength;
	tUSART_NPT_TransmitCallback *pTransmitCallback;
	tUSART_NPT_ReceiveCallback *pReceiveCallback;
	unsigned long CR1InitialValue;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	BOOL IsSharedUSART3_4IRQ;
	unsigned char Padding[3];
#endif
} sUSART_NPT_PortVars;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	void USART1_IRQHandler(void);
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	void USART2_IRQHandler(void);
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4)
	void USART3_4_IRQHandler(void);
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	void USART3_4_LPUART1_IRQHandler(void);
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	static void USART_IDLEIRQHandler(unsigned long PortNb);
#endif
static void USART_RTOIRQHandler(unsigned long PortNb);
static void USART_TCIRQHandler(unsigned long PortNb);

#endif // !dedined(USART_NEW_PORT_PRIVATE_H)
