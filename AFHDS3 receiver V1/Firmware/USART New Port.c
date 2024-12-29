#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "New Port.h"
#include "USART.h"

#include "USART New Port.h"

#include "USART New Port private.h"

#define USART_NPT_CR3_INITIAL_VALUE (USART_CR3_ONEBIT | USART_CR3_OVRDIS)

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	#define NPT_USART1_PORT_NB 3
	#define NPT_USART2_PORT_NB 1
	#define NPT_USART3_PORT_NB 2
	#define NPT_USART4_PORT_NB 0
#elif defined(PN_INR6_HS)  || defined(PN_INR6_FC) || defined(PN_FGR4D)
	#define NPT_USART1_PORT_NB 0
	#define NPT_USART2_PORT_NB 1
	#define NPT_USART3_PORT_NB 2
	#define NPT_USART4_PORT_NB 3
#elif defined(PN_TR8B)
	#define NPT_USART1_PORT_NB 0
	#define NPT_USART4_PORT_NB 1
#elif defined(PN_TMR)
	#define NPT_USART1_PORT_NB 0
#endif

static BOOL USART_NPT_Initialized[SES_NPT_NB_PORTS];
static BOOL USART_NPT_IDLEInterruptsEnabled[SES_NPT_NB_PORTS];
static BOOL USART_NPT_Transmitting[SES_NPT_NB_PORTS];

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
static sUSART_NPT_PortVars USART_NPT_PortsVars[SES_NPT_NB_PORTS]={
	{
		.pRegs=USART4,
		.pDMAChannel=DMA1_Channel4,
		.IRQNb=USART3_4_IRQn,
		.DMAMUXChannelNb=3,
		.DMAMUXTXInputNb=57,
		.DMAMUXRXInputNb=56,
		.IsSharedUSART3_4IRQ=TRUE
	},
	{
		.pRegs=USART2,
		.pDMAChannel=DMA1_Channel2,
		.IRQNb=USART2_IRQn,
		.DMAMUXChannelNb=1,
		.DMAMUXTXInputNb=53,
		.DMAMUXRXInputNb=52,
		.IsSharedUSART3_4IRQ=FALSE
	},
	{
		.pRegs=USART3,
		.pDMAChannel=DMA1_Channel3,
		.IRQNb=USART3_4_IRQn,
		.DMAMUXChannelNb=2,
		.DMAMUXTXInputNb=55,
		.DMAMUXRXInputNb=54,
		.IsSharedUSART3_4IRQ=TRUE
	},
	{
		.pRegs=USART1,
		.pDMAChannel=DMA1_Channel1,
		.IRQNb=USART1_IRQn,
		.DMAMUXChannelNb=0,
		.DMAMUXTXInputNb=51,
		.DMAMUXRXInputNb=50,
		.IsSharedUSART3_4IRQ=FALSE
	}
};
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
static sUSART_NPT_PortVars USART_NPT_PortsVars[SES_NPT_NB_PORTS]={
	{
		.pRegs=USART1,
		.pDMAChannel=DMA1_Channel1,
		.IRQNb=USART1_IRQn,
		.DMAMUXChannelNb=0,
		.DMAMUXTXInputNb=51,
		.DMAMUXRXInputNb=50,
		.IsSharedUSART3_4IRQ=FALSE
	},
	{
		.pRegs=USART2,
		.pDMAChannel=DMA1_Channel2,
		.IRQNb=USART2_IRQn,
		.DMAMUXChannelNb=1,
		.DMAMUXTXInputNb=53,
		.DMAMUXRXInputNb=52,
		.IsSharedUSART3_4IRQ=FALSE
	},
	{
		.pRegs=USART3,
		.pDMAChannel=DMA1_Channel3,
		.IRQNb=USART3_4_LPUART1_IRQn,
		.DMAMUXChannelNb=2,
		.DMAMUXTXInputNb=55,
		.DMAMUXRXInputNb=54,
		.IsSharedUSART3_4IRQ=TRUE
	},
	{
		.pRegs=USART4,
		.pDMAChannel=DMA1_Channel4,
		.IRQNb=USART3_4_LPUART1_IRQn,
		.DMAMUXChannelNb=3,
		.DMAMUXTXInputNb=57,
		.DMAMUXRXInputNb=56,
		.IsSharedUSART3_4IRQ=TRUE
	}
};
#elif defined(PN_TR8B)
static sUSART_NPT_PortVars USART_NPT_PortsVars[SES_NPT_NB_PORTS]={
	{
		.pRegs=USART1,
		.pDMAChannel=DMA1_Channel1,
		.IRQNb=USART1_IRQn,
		.DMAMUXChannelNb=0,
		.DMAMUXTXInputNb=51,
		.DMAMUXRXInputNb=50,
		.IsSharedUSART3_4IRQ=FALSE
	},
	{
		.pRegs=USART4,
		.pDMAChannel=DMA1_Channel4,
		.IRQNb=USART3_4_LPUART1_IRQn,
		.DMAMUXChannelNb=3,
		.DMAMUXTXInputNb=57,
		.DMAMUXRXInputNb=56,
		.IsSharedUSART3_4IRQ=TRUE
	}
};
#elif defined(PN_TMR)
static sUSART_NPT_PortVars USART_NPT_PortsVars[SES_NPT_NB_PORTS]={
	{
		.pRegs=USART1,
		.pDMAChannel=DMA1_Channel1,
		.IRQNb=USART1_IRQn,
		.DMAMUXChannelNb=0,
		.DMAMUXTXInputNb=51,
		.DMAMUXRXInputNb=50,
	}
};
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	static unsigned char USART_NPT_IsUSART3_4IRQPriorityHigh; // 1 bit per port
#endif

const unsigned char USART_NPT_ReceiveTimeDelay=9; // In microseconds
const unsigned char USART_NPT_TransmitTimeDelay=9; // In microseconds
const unsigned char USART_NPT_TransmitCompleteTimeDelay=15; // In microseconds

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
void USART1_IRQHandler(void)
{
	if (USART1->ISR&USART_ISR_RTOF)
		USART_RTOIRQHandler(NPT_USART1_PORT_NB);
	if (USART1->ISR&USART_ISR_TC)
		USART_TCIRQHandler(NPT_USART1_PORT_NB);
	return;
}
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
void USART2_IRQHandler(void)
{
	if (USART2->ISR&USART_ISR_RTOF)
		USART_RTOIRQHandler(NPT_USART2_PORT_NB);
	if (USART2->ISR&USART_ISR_TC)
		USART_TCIRQHandler(NPT_USART2_PORT_NB);
	return;
}
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8)\
	|| defined(PN_FBR4)|| defined(PN_FBR8) || defined(PN_FBR4)
void USART3_4_IRQHandler(void)
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
void USART3_4_LPUART1_IRQHandler(void)
#endif
{
#if defined(PN_TR8B)
	if (USART_NPT_Initialized[NPT_USART4_PORT_NB] &&
		USART4->ISR&USART_ISR_IDLE)
		USART_IDLEIRQHandler(NPT_USART4_PORT_NB);
	if (USART_NPT_Initialized[NPT_USART4_PORT_NB] &&
		USART4->ISR&USART_ISR_TC)
		USART_TCIRQHandler(NPT_USART4_PORT_NB);
#else
	if (USART_NPT_Initialized[NPT_USART3_PORT_NB] &&
		USART3->ISR&USART_ISR_IDLE)
		USART_IDLEIRQHandler(NPT_USART3_PORT_NB);
	if (USART_NPT_Initialized[NPT_USART4_PORT_NB] &&
		USART4->ISR&USART_ISR_IDLE)
		USART_IDLEIRQHandler(NPT_USART4_PORT_NB);
	if (USART_NPT_Initialized[NPT_USART3_PORT_NB] &&
		USART3->ISR&USART_ISR_TC)
		USART_TCIRQHandler(NPT_USART3_PORT_NB);
	if (USART_NPT_Initialized[NPT_USART4_PORT_NB] &&
		USART4->ISR&USART_ISR_TC)
		USART_TCIRQHandler(NPT_USART4_PORT_NB);
#endif
	return;
}
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
static void USART_IDLEIRQHandler(unsigned long PortNb)
{
	tUSART_NPT_ReceiveCallback *pCallback;
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// Clear the IDLE flag
	pPortVars->pRegs->ICR=USART_ICR_IDLECF;
	if (!USART_NPT_IDLEInterruptsEnabled[PortNb])
		return;
	pCallback=pPortVars->pReceiveCallback;
	if (pCallback)
	{
		unsigned long NbReceivedBytes;
		
		NbReceivedBytes=pPortVars->ReceiveDataMaxLength-pPortVars->pDMAChannel->CNDTR;
		if (NbReceivedBytes)
		{
			USART_NPT_IDLEInterruptsEnabled[PortNb]=FALSE;
			// RE=0 Receiver disabled
			// TE=0 Transmitter disabled
			// TCIE=0 Transmission complete interrupt disabled
			// RTOIE=0 Receiver timeout interrupt disabled
			pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
			// Disable TX and RX DMA
			pPortVars->pDMAChannel->CCR=0;
			pPortVars->pDMAChannel->CCR=0;
			pPortVars->pReceiveCallback=NULL;
			pCallback(NbReceivedBytes);
		}
	}
	return;
}
#endif

static void USART_RTOIRQHandler(unsigned long PortNb)
{
	tUSART_NPT_ReceiveCallback *pCallback;
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// Clear the receive timeout flag
	pPortVars->pRegs->ICR=USART_ICR_RTOCF;
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// TCIE=0 Transmission complete interrupt disabled
	// RTOIE=0 Receiver timeout interrupt disabled
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	// Disable TX and RX DMA
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	pCallback=pPortVars->pReceiveCallback;
	if (pCallback)
	{
		pPortVars->pReceiveCallback=NULL;
		pCallback(pPortVars->ReceiveDataMaxLength-pPortVars->pDMAChannel->CNDTR);
	}
	return;
}

static void USART_TCIRQHandler(unsigned long PortNb)
{
	tUSART_NPT_TransmitCallback *pCallback;
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// Clear the Transmit complete flag
	pPortVars->pRegs->ICR=USART_ICR_TCCF;
	if (!USART_NPT_Transmitting[PortNb])
		return;
	USART_NPT_Transmitting[PortNb]=FALSE;
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// TCIE=0 Transmission complete interrupt disabled
	// RTOIE=0 Receiver timeout interrupt disabled
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	// Disable TX and RX DMA
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	pCallback=pPortVars->pTransmitCallback;
	if (pCallback)
	{
		pPortVars->pTransmitCallback=NULL;
		pCallback();
	}
	return;
}

void USART_NPT_Deinit(unsigned long PortNb)
{
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// TCIE=0 Transmission complete interrupt disabled
	// RTOIE=0 Receiver timeout interrupt disabled
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	// Disable TX and RX DMA
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	USART_NPT_Transmitting[PortNb]=FALSE;
	// Set USARTx_TX as low level output
	SYS_DisableIRQs();
	switch (PortNb)
	{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	case 0:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (0<<(1*2));
		GPIOA->BSRR=1<<(16+ 0);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(0*2)) | (3<<(1*2)))) | (0<<(0*2)) | (0<<(1*2));
		break;
	case 1:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(3*2))) | (0<<(3*2));
		GPIOA->BSRR=1<<(16+ 2);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(2*2)) | (3<<(3*2)))) | (0<<(2*2)) | (0<<(3*2));
		break;
	case 2:
		GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (0<<(0*2));
		GPIOA->BSRR=1<<(16+ 5);
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(5*2))) | (0<<(5*2));
		GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (0<<(0*2));
		break;
	case 3:
		GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(7*2))) | ( 0<<(7*2));
		GPIOB->BSRR=1<<(16+ 6);
		GPIOB->MODER=(GPIOB->MODER&~((3UL<<(6*2)) | (3<<(7*2)))) | (0<<(6*2)) | (0<<(7*2));
		break;
#elif defined(PN_INR6_AS) || defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	case 0:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (0<<(10*2));
		GPIOA->BSRR=1<<(16+ 9);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(9*2)) | (3<<(10*2)))) | (0<<(9*2)) | (0<<(10*2));
		break;
	case 1:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(15*2))) | (0<<(15*2));
		GPIOA->BSRR=1<<(16+ 2);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(2*2)) | (3UL<<(15*2)))) | (0<<(2*2)) | (0<<(15*2));
		break;
	case 2:
		GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | ( 0<<(0*2));
		GPIOB->BSRR=1<<(16+ 2);
		GPIOB->MODER=(GPIOB->MODER&~((3UL<<(2*2)) | (3<<(0*2)))) | (0<<(2*2)) | (0<<(0*2));
		break;
	case 3:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (0<<(1*2));
		GPIOA->BSRR=1<<(16+ 0);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(0*2)) | (3<<(1*2)))) | (0<<(0*2)) | (0<<(1*2));
		break;
#elif defined(PN_TR8B)
	case 0:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (0<<(10*2));
		GPIOA->BSRR=1<<(16+ 9);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(9*2)) | (3<<(10*2)))) | (0<<(9*2)) | (0<<(10*2));
		break;
	case 1:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (0<<(1*2));
		GPIOA->BSRR=1<<(16+ 0);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(0*2)) | (3<<(1*2)))) | (0<<(0*2)) | (0<<(1*2));
		break;
#elif defined(PN_TMR)
	case 0:
		GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (0<<(10*2));
		GPIOA->BSRR=1<<(16+ 9);
		GPIOA->MODER=(GPIOA->MODER&~((3UL<<(9*2)) | (3<<(10*2)))) | (0<<(9*2)) | (0<<(10*2));
		break;
#endif
	}
	SYS_EnableIRQs();
	return;
}

unsigned long USART_NPT_GetNbReceivedBytes(unsigned long PortNb)
{
	unsigned long CNDTR;
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	CNDTR=pPortVars->pDMAChannel->CNDTR;
	if (pPortVars->ReceiveDataMaxLength>CNDTR)
		return pPortVars->ReceiveDataMaxLength-CNDTR;
	else
		return 0;
}

// May be called several times
void USART_NPT_Init(unsigned long PortNb,const sUSART_Config *pUSARTConfig,BOOL IsIRQPriorityHigh)
{
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// Disable the USART1 IRQ
	NVIC_DisableIRQ(pPortVars->IRQNb);
	USART_NPT_IDLEInterruptsEnabled[PortNb]=FALSE;
	// Enable and reset the USART module clock
	switch (PortNb)
	{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	case 0:
		RCC->APBENR1|=RCC_APBENR1_USART4EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART4RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART4RST;
		break;
	case 1:
		RCC->APBENR1|=RCC_APBENR1_USART2EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART2RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART2RST;
		break;
	case 2:
		RCC->APBENR1|=RCC_APBENR1_USART3EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART3RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART3RST;
		break;
	case 3:
		RCC->APBENR2|=RCC_APBENR2_USART1EN;
		RCC->APBRSTR2|=RCC_APBRSTR2_USART1RST;
		RCC->APBRSTR2&=~RCC_APBRSTR2_USART1RST;
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	case 0:
		RCC->APBENR2|=RCC_APBENR2_USART1EN;
		RCC->APBRSTR2|=RCC_APBRSTR2_USART1RST;
		RCC->APBRSTR2&=~RCC_APBRSTR2_USART1RST;
		break;
	case 1:
		RCC->APBENR1|=RCC_APBENR1_USART2EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART2RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART2RST;
		break;
	case 2:
		RCC->APBENR1|=RCC_APBENR1_USART3EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART3RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART3RST;
		break;
	case 3:
		RCC->APBENR1|=RCC_APBENR1_USART4EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART4RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART4RST;
		break;
#elif defined(PN_TR8B)
	case 0:
		RCC->APBENR2|=RCC_APBENR2_USART1EN;
		RCC->APBRSTR2|=RCC_APBRSTR2_USART1RST;
		RCC->APBRSTR2&=~RCC_APBRSTR2_USART1RST;
		break;
	case 1:
		RCC->APBENR1|=RCC_APBENR1_USART4EN;
		RCC->APBRSTR1|=RCC_APBRSTR1_USART4RST;
		RCC->APBRSTR1&=~RCC_APBRSTR1_USART4RST;
		break;
#elif defined(PN_TMR)
	case 0:
		RCC->APBENR2|=RCC_APBENR2_USART1EN;
		RCC->APBRSTR2|=RCC_APBRSTR2_USART1RST;
		RCC->APBRSTR2&=~RCC_APBRSTR2_USART1RST;
		break;
#endif
	}
	// Disable TX and RX DMA
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	// Clear the transfer complete flag
	pPortVars->pRegs->ICR=USART_ICR_TCCF;
	pPortVars->ReceiveDataMaxLength=0;
	pPortVars->pTransmitCallback=NULL;
	pPortVars->pReceiveCallback=NULL;
	// UE=0 USART disabled
	// UESM=0 USART not able to wake up the MCU from low-power mode
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// IDLEIE=1 IDLE interrupt enabled for USART3 and USART4
	// RXNEIE=0 Receive data register not empty interrupt disabled
	// TCIE=0 Transmission complete interrupt disabled
	// TXEIE=0 TX empty interrupt disabled
	// PEIE=0 Parity error interrupt disabled
	// PS=0 or 1 depending on Parity
	// PCE=0 or 1 Parity control enabled or disabled depending on Parity
	// WAKE=0 Not used
	// M0=0 or 1 8 or 9 bit word length depending on parity
	// MME=0 Mute mode disabled
	// CMIE=0  Character match interrupt disabled
	// OVER8=0 Oversampling by 16
	// DEDT=0 Driver Enable deassertion time is null
	// DEAT=0 Driver Enable assertion time is null
	// RTOIE=0 Receiver timeout interrupt disabled
	// EOBIE=0 End of Block interrupt disabled
	// M1=0 1 start, 8 or 9 data bits
	// FIFOEN=0 FIFO mode is disabled
	pPortVars->CR1InitialValue=((pUSARTConfig->Parity!=USART_PARITY_NONE)?USART_CR1_PCE:0) |
		((pUSARTConfig->Parity==USART_PARITY_ODD)?USART_CR1_PS:0) |
		((pUSARTConfig->Parity==USART_PARITY_NONE)?0:USART_CR1_M0);
	if (pPortVars->DMAMUXChannelNb>=2)
		pPortVars->CR1InitialValue|=USART_CR1_IDLEIE;
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	{
		unsigned long CR2;

		// SLVEN=0 Slave mode disabled
		// DIS_NSS=0 SPI slave selection depends on NSS input pin
		// ADDM7=0 Not used
		// LBDL=0 LIN break detection not used
		// LBDIE=0 LIN break detection interrupt disabled
		// LBCL=0 Not used
		// CPHA=0 Not used
		// CPOL=0 Not used
		// CLKEN=0 SCLK pin disabled
		// STOP depends on StopBit
		// LINEN=0 LIN mode disabled
		// SWAP=0 RX and TX lines not swapped
		// RXINV depends on IsInverted
		// TXINV depends on IsInverted
		// DATAINV=0 Data not inverted
		// MSBFIRST=0 LSB first
		// ABREN=0 Auto baud rate disabled
		// ABRMOD=0 Not used
		// RTOEN=1 Receiver timeout feature enabled
		// ADD=0 Not used
		CR2=((pUSARTConfig->StopBits==USART_STOPBIT_1)?0:USART_CR2_STOP_1) |
			((pUSARTConfig->StopBits==USART_STOPBIT_1_5)?USART_CR2_STOP_0:0) |
			((pUSARTConfig->Inverted)?USART_CR2_RXINV:0) |
			((pUSARTConfig->Inverted)?USART_CR2_TXINV:0);
		// Receive timeout after 10 data bits
		if (pPortVars->DMAMUXChannelNb<=1)
		{
			CR2|=USART_CR2_RTOEN;
			pPortVars->pRegs->RTOR=10;
		}
		pPortVars->pRegs->CR2=CR2;
	}
	pPortVars->pRegs->CR3=USART_NPT_CR3_INITIAL_VALUE;
	// Set GPIOs to their respective USART_TX and USART_RX functions
	// Set the pull-up or pull-down depending on USART inversion
	// Set USART_TX and USART_RX GPIOs as alternate function
	{
		unsigned long PullUpDownType;
		
		PullUpDownType=(!pUSARTConfig->Inverted)?1:2;
		SYS_DisableIRQs();
		switch (PortNb)
		{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
		case 0:
			GPIOA->AFR[0]=(GPIOA->AFR[0]&0xFFFFFF00) | 0x00000044;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (PullUpDownType<<(1*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(0*2)) | (3<<(1*2)))) | (2<<(0*2)) | (2<<(1*2));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(15*2))) | (0<<(15*2));
			break;
		case 1:
			GPIOA->AFR[0]=(GPIOA->AFR[0]&0xFFFF00FF) | 0x00001100;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(3*2))) | (PullUpDownType<<(3*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(2*2)) | (3<<(3*2)))) | (2<<(2*2)) | (2<<(3*2));
			break;
		case 2:
			GPIOA->AFR[0]=(GPIOA->AFR[0]&0xFF0FFFFF) | 0x00400000;
			GPIOB->AFR[0]=(GPIOB->AFR[0]&0xFFFFFFF0) | 0x00000004;
			GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (PullUpDownType<<(0*2));
			GPIOA->MODER=(GPIOA->MODER&~(3UL<<(5*2))) | (2<<(5*2));
			GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (2<<(0*2));
			break;
		case 3:
			GPIOB->AFR[0]=(GPIOB->AFR[0]&0x00FFFFFF) | 0x00000000;
			GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(7*2))) | ( PullUpDownType<<(7*2));
			GPIOB->MODER=(GPIOB->MODER&~((3UL<<(6*2)) | (3<<(7*2)))) | (2<<(6*2)) | (2<<(7*2));
			break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
		case 0:
			GPIOA->AFR[1]=(GPIOA->AFR[1]&0xFFFFF00F) | 0x00000110;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (PullUpDownType<<(10*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(9*2)) | (3<<(10*2)))) | (2<<(9*2)) | (2<<(10*2));
			break;
		case 1:
			GPIOA->AFR[0]=(GPIOA->AFR[0]&0xFFFFF0FF) | 0x00000100;
			GPIOA->AFR[1]=(GPIOA->AFR[1]&0x0FFFFFFF) | 0x10000000;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(15*2))) | (PullUpDownType<<(15*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(2*2)) | (3UL<<(15*2)))) | (2<<(2*2)) | (2UL<<(15*2));
			break;
		case 2:
			GPIOB->AFR[0]=(GPIOB->AFR[0]&0xFFFFF0F0) | 0x00000404;
			GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | ( PullUpDownType<<(0*2));
			GPIOB->MODER=(GPIOB->MODER&~((3UL<<(2*2)) | (3<<(0*2)))) | (2<<(2*2)) | (2<<(0*2));
			break;
		case 3:
			GPIOA->AFR[0]=(GPIOA->AFR[0]&0xFFFFFF00) | 0x00000044;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (PullUpDownType<<(1*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(0*2)) | (3<<(1*2)))) | (2<<(0*2)) | (2<<(1*2));
			break;
#elif defined(PN_TR8B)
		case 0:
			GPIOA->AFR[1]=(GPIOA->AFR[1]&0xFFFFF00F) | 0x00000110;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (PullUpDownType<<(10*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(9*2)) | (3<<(10*2)))) | (2<<(9*2)) | (2<<(10*2));
			break;
		case 1:
			GPIOA->AFR[0]=(GPIOA->AFR[0]&0xFFFFFF00) | 0x00000044;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (PullUpDownType<<(1*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(0*2)) | (3<<(1*2)))) | (2<<(0*2)) | (2<<(1*2));
			break;
#elif defined(PN_TMR)
		case 0:
			GPIOA->AFR[1]=(GPIOA->AFR[1]&0xFFFFF00F) | 0x00000110;
			GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (PullUpDownType<<(10*2));
			GPIOA->MODER=(GPIOA->MODER&~((3UL<<(9*2)) | (3<<(10*2)))) | (2<<(9*2)) | (2<<(10*2));
			break;
#endif
		}
		SYS_EnableIRQs();
	}
	// Set the baud rate
	pPortVars->pRegs->BRR=(pUSARTConfig->BitDuration*((SYSCLK/1000000)*(1<<16)/500)+(1<<15))>>16;
	// Enable the USART
	pPortVars->CR1InitialValue|=USART_CR1_UE;
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	// Clear the IDLE, the transmission complete and the receiver timeout flags
	pPortVars->pRegs->ICR=USART_ICR_IDLECF | USART_ICR_TCCF | USART_ICR_RTOCF;
	// Clear the USART IRQ pending flag
	NVIC_ClearPendingIRQ(pPortVars->IRQNb);
	// Set the USART IRQ to the required priority
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// USART 3 and 4 share the same IRQ handler
	if (USART_NPT_PortsVars->IsSharedUSART3_4IRQ)
	{
		USART_NPT_IsUSART3_4IRQPriorityHigh=
			(unsigned char)((USART_NPT_IsUSART3_4IRQPriorityHigh & ~(1<<PortNb)) | ((IsIRQPriorityHigh)?(1<<PortNb):0));
		IsIRQPriorityHigh=(USART_NPT_IsUSART3_4IRQPriorityHigh)?TRUE:FALSE;
	}
#endif
	USART_NPT_Initialized[PortNb]=TRUE;
	NVIC_SetPriority(pPortVars->IRQNb,(IsIRQPriorityHigh)?IRQ_PRI_REALTIME:IRQ_PRI_MEDIUM);
	// Enable the USART IRQ
	NVIC_EnableIRQ(pPortVars->IRQNb);
	return;
}

void USART_NPT_ReceiveSetup(unsigned long PortNb,const void *pData,unsigned long DataLength,BOOL IsCircular,tUSART_NPT_ReceiveCallback *pCallback)
{
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	pPortVars->pReceiveCallback=pCallback;
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// TCIE=0 Transmission complete interrupt disabled
	// RTOIE=0 Receiver timeout interrupt disabled
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	// Disable TX and RX DMA
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	SYS_DisableIRQs();
	// Set USARTx_TX as input
	switch (PortNb)
	{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (0<<(0*2));
		break;
	case 1:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (0<<(2*2));
		break;
	case 2:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(5*2))) | (0<<(5*2));
		break;
	case 3:
		GPIOB->MODER=(GPIOB->MODER&~(3UL<<(6*2))) | (0<<(6*2));
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (0<<(9*2));
		break;
	case 1:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (0<<(2*2));
		break;
	case 2:
		GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (0<<(2*2));
		break;
	case 3:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (0<<(0*2));
		break;
#elif defined(PN_TR8B)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (0<<(9*2));
		break;
	case 1:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (0<<(0*2));
		break;
#elif defined(PN_TMR)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (0<<(9*2));
		break;
#endif
	}
	SYS_EnableIRQs();
	// Set the DMAMUX
	// DMAREQ_ID=x Select the request source peripheral
	// SOIE=0 Synchronization overrun interrupt disabled
	// EGE=0 Event generation disabled
	// SE=0 Synchronization disabled
	// SPOL=0 Not used
	// NBREQ=0 Not used
	// SYNC_ID=0 Not used
	DMAMUX1[pPortVars->DMAMUXChannelNb].CCR=(unsigned long)pPortVars->DMAMUXRXInputNb<<DMAMUX_CxCR_DMAREQ_ID_Pos;
	// Set up the RX DMA
	pPortVars->pDMAChannel->CPAR=(unsigned long)&pPortVars->pRegs->RDR;
	pPortVars->pDMAChannel->CMAR=(unsigned long)pData;
	pPortVars->pDMAChannel->CNDTR=DataLength;
	pPortVars->ReceiveDataMaxLength=DataLength;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=0 Read from peripheral
	// CIRC=x Circular mode enabled depending on IsCircular
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	if (IsCircular)
	{
		pPortVars->pDMAChannel->CCR=             DMA_CCR_CIRC | DMA_CCR_MINC;
		pPortVars->pDMAChannel->CCR=DMA_CCR_EN | DMA_CCR_CIRC | DMA_CCR_MINC;
	}
	else
	{
		pPortVars->pDMAChannel->CCR=             DMA_CCR_MINC;
		pPortVars->pDMAChannel->CCR=DMA_CCR_EN | DMA_CCR_MINC;
	}
	// Clear the reception buffer
	while (pPortVars->pRegs->ISR&USART_ISR_RXNE_RXFNE)
		pPortVars->pRegs->RDR;
	// Enable RX DMA
	pPortVars->pRegs->CR3=USART_NPT_CR3_INITIAL_VALUE | USART_CR3_DMAR;
	return;
}

void USART_NPT_ReceiveStart(unsigned long PortNb)
{
	unsigned long CR1;
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// Enable reception
	CR1=pPortVars->CR1InitialValue | USART_CR1_RE;
	if (pPortVars->pReceiveCallback)
	{
		if (pPortVars->DMAMUXChannelNb<=1)
		{
			// Clear the receive timeout flag
			pPortVars->pRegs->ICR=USART_ICR_RTOCF;
			CR1|=USART_CR1_RTOIE;
		}
		else
			USART_NPT_IDLEInterruptsEnabled[PortNb]=TRUE;
	}
	pPortVars->pRegs->CR1=CR1;
	return;
}

void USART_NPT_ReceiveStop(unsigned long PortNb)
{
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// TCIE=0 Transmission complete interrupt disabled
	// RTOIE=0 Receiver timeout interrupt disabled
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	return;
}

void USART_NPT_TransmitSetup(unsigned long PortNb,const void *pData,unsigned long DataLength,tUSART_NPT_TransmitCallback *pCallback)
{
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	USART_NPT_IDLEInterruptsEnabled[PortNb]=FALSE;
	pPortVars->pTransmitCallback=pCallback;
	// RE=0 Receiver disabled
	// TE=0 Transmitter disabled
	// TCIE=0 Transmission complete interrupt disabled
	// RTOIE=0 Receiver timeout interrupt disabled
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue;
	// Disable TX and RX DMA
	pPortVars->pDMAChannel->CCR=0;
	pPortVars->pDMAChannel->CCR=0;
	// Set USARTx_TX as alternate function
	SYS_DisableIRQs();
	switch (PortNb)
	{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (2<<(0*2));
		break;
	case 1:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (2<<(2*2));
		break;
	case 2:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(5*2))) | (2<<(5*2));
		break;
	case 3:
		GPIOB->MODER=(GPIOB->MODER&~(3UL<<(6*2))) | (2<<(6*2));
		break;
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (2<<(9*2));
		break;
	case 1:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (2<<(2*2));
		break;
	case 2:
		GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (2<<(2*2));
		break;
	case 3:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (2<<(0*2));
		break;
#elif defined(PN_TR8B)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (2<<(9*2));
		break;
	case 1:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (2<<(0*2));
		break;
#elif defined(PN_TMR)
	case 0:
		GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (2<<(9*2));
		break;
#endif
	}
	SYS_EnableIRQs();
	// Set up the TX DMA
	// Set the DMAMUX
	// DMAREQ_ID=x Select the request source peripheral
	// SOIE=0 Synchronization overrun interrupt disabled
	// EGE=0 Event generation disabled
	// SE=0 Synchronization disabled
	// SPOL=0 Not used
	// NBREQ=0 Not used
	// SYNC_ID=0 Not used
	DMAMUX1[pPortVars->DMAMUXChannelNb].CCR=(unsigned long)(pPortVars->DMAMUXTXInputNb<<DMAMUX_CxCR_DMAREQ_ID_Pos);
	pPortVars->pDMAChannel->CPAR=(unsigned long)&pPortVars->pRegs->TDR;
	pPortVars->pDMAChannel->CMAR=(unsigned long)pData;
	pPortVars->pDMAChannel->CNDTR=DataLength;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=1 Read from memory
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	pPortVars->pDMAChannel->CCR=             DMA_CCR_DIR | DMA_CCR_MINC;
	pPortVars->pDMAChannel->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
	// Enable TX DMA
	pPortVars->pRegs->CR3=USART_NPT_CR3_INITIAL_VALUE | USART_CR3_DMAT;
	return;
}

void USART_NPT_TransmitStart(unsigned long PortNb)
{
	sUSART_NPT_PortVars *pPortVars;

	pPortVars=&USART_NPT_PortsVars[PortNb];
	USART_NPT_Transmitting[PortNb]=TRUE;
	// Enable transmission and the transmit complete interrupt
	pPortVars->pRegs->CR1=pPortVars->CR1InitialValue | USART_CR1_TE | USART_CR1_TCIE;
	return;
}
