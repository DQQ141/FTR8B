#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "PHY SX1280.h"

#include "SPI SX1280.h"

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	#define SPIX SPI2
	#define SPI_DMA_CHANNEL_RX DMA1_Channel6
	#define SPI_DMA_CHANNEL_TX DMA1_Channel5
	#define DMA_IFCR_CTCIFX_RX DMA_IFCR_CTCIF6
	#define DMA_IFCR_CTCIFX_TX DMA_IFCR_CTCIF5
	#define DMA_ISR_TCIFX_RX DMA_ISR_TCIF6
	#define DMA_ISR_TCIFX_TX DMA_ISR_TCIF5
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	#define SPIX SPI1
	#define SPI_DMA_CHANNEL_RX DMA1_Channel6
	#define SPI_DMA_CHANNEL_TX DMA1_Channel5
	#define DMA_IFCR_CTCIFX_RX DMA_IFCR_CTCIF6
	#define DMA_IFCR_CTCIFX_TX DMA_IFCR_CTCIF5
	#define DMA_ISR_TCIFX_RX DMA_ISR_TCIF6
	#define DMA_ISR_TCIFX_TX DMA_ISR_TCIF5
#elif defined(PN_GMR)
	#define SPIX SPI1
	#define SPI_DMA_CHANNEL_RX DMA1_Channel2
	#define SPI_DMA_CHANNEL_TX DMA1_Channel3
	#define DMA_IFCR_CTCIFX_RX DMA_IFCR_CTCIF2
	#define DMA_IFCR_CTCIFX_TX DMA_IFCR_CTCIF3
	#define DMA_ISR_TCIFX_RX DMA_ISR_TCIF2
	#define DMA_ISR_TCIFX_TX DMA_ISR_TCIF3
#endif

#define SPI_CR2_INIT_VALUE ((SPI_CR2_DS_0*7) | SPI_CR2_FRXTH)

static tPHY_BusyIRQCallback *pSPI_BusyIRQCallback;
static BOOL SPI_IsTXAndRXDMA;

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void)
#elif defined(PN_GMR)
	void DMA1_Channel2_3_IRQHandler(void)
#endif
{
	unsigned long ISR;

	ISR=DMA1->ISR;
	// TX end
	if (ISR & DMA_ISR_TCIFX_TX)
	{
		// Clear DMA transfer complete flag
		DMA1->IFCR=DMA_IFCR_CTCIFX_TX;
		if (!SPI_IsTXAndRXDMA)
		{
			// Wait for the end of the transfer
			while (SPIX->SR&SPI_SR_BSY);
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// CS=1
	GPIOB->BSRR=1<<12;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// CS=1
	GPIOA->BSRR=1<<4;
#elif defined(PN_GMR)
	// CS=1
	GPIOA->BSRR=1<<15;
#endif
			// Disable TX DMA
			SPI_DMA_CHANNEL_TX->CCR=0;
			SPI_DMA_CHANNEL_TX->CCR=0;
			// Disable SPI RX and TX DMA
			SPIX->CR2=SPI_CR2_INIT_VALUE;
			// Flush the RX buffer
			while (SPIX->SR&SPI_SR_RXNE)
				*(volatile unsigned char *)&SPIX->DR;
			// Clear the overrun flag
			SPIX->SR;
		}
		return;
	}
	// RX end (with TX)
	if (ISR & DMA_ISR_TCIFX_RX)
	{
		// Clear DMA transfer complete flags
		DMA1->IFCR=DMA_IFCR_CTCIFX_RX;
		// Wait for the end of the transfer
		while (SPIX->SR&SPI_SR_BSY);
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// CS=1
	GPIOB->BSRR=1<<12;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// CS=1
	GPIOA->BSRR=1<<4;
#elif defined(PN_GMR)
	// CS=1
	GPIOA->BSRR=1<<15;
#endif
		__DSB();
		// Disable TX and RX DMA
		SPI_DMA_CHANNEL_TX->CCR=0;
		SPI_DMA_CHANNEL_TX->CCR=0;
		SPI_DMA_CHANNEL_RX->CCR=0;
		SPI_DMA_CHANNEL_RX->CCR=0;
		// Disable SPI RX and TX DMA
		SPIX->CR2=SPI_CR2_INIT_VALUE;
		return;
	}
	return;
}

void SPI_SX1280_BusyIRQHandler(void)
{
	tPHY_BusyIRQCallback *pBusyIRQCallback;

	pBusyIRQCallback=pSPI_BusyIRQCallback;
	if (pBusyIRQCallback)
	{
		pSPI_BusyIRQCallback=NULL;
		pBusyIRQCallback();
	}
	return;
}

void SPI_Init(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set PB12 to high speed GPIO output (SPI CS)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(12*2))) | (1<<(12*2));
	GPIOB->OSPEEDR=(GPIOB->OSPEEDR&~(3UL<<(12*2))) | (3<<(12*2));
	// Set PB10, PB2 and PB11 to high speed alternate SPI SCK, SPI MISO and SPI MOSI mode
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~(0xFUL<<(2*4))) | (1<<(2*4));
	GPIOB->AFR[1]=(GPIOB->AFR[1]&~((0xFUL<<((10-8)*4)) | (0xF<<((11-8)*4)))) | (5<<((10-8)*4)) | (0<<((11-8)*4));
	GPIOB->OSPEEDR=(GPIOB->OSPEEDR&~((3UL<<(10*2)) | (3<<(2*2)) | (3<<(11*2)))) | (3<<(10*2)) | (3<<(2*2)) | (3<<(11*2));
	GPIOB->MODER=(GPIOB->MODER&~((3UL<<(10*2)) | (3<<(2*2)) | (3<<(11*2)))) | (2<<(10*2)) | (2<<(2*2)) | (2<<(11*2));
	// Enable the SPI2 module clock
	RCC->APBENR1|=RCC_APBENR1_SPI2EN;
	// Reset SPI2
	RCC->APBRSTR1|=RCC_APBRSTR1_SPI2RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_SPI2RST;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// Set PA4 to high speed GPIO output (SPI CS)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(4*2))) | (1<<(4*2));
	GPIOA->OSPEEDR=(GPIOA->OSPEEDR&~(3UL<<(4*2))) | (3<<(4*2));
	// Set PA5, PA6 and PA7 to high speed alternate SPI SCK, SPI MISO and SPI MOSI mode
	GPIOA->AFR[0]=(GPIOA->AFR[0]&~((0xFUL<<(5*4)) | (0xFUL<<(6*4)) | (0xFU<<(7*4)))) | (0<<(5*4)) | (0<<(6*4)) | (0<<(7*4));
	GPIOA->OSPEEDR=(GPIOA->OSPEEDR&~((3UL<<(5*2)) | (3<<(6*2)) | (3<<(7*2)))) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2));
	GPIOA->MODER=(GPIOA->MODER&~((3UL<<(5*2)) | (3<<(6*2)) | (3<<(7*2)))) | (2<<(5*2)) | (2<<(6*2)) | (2<<(7*2));
	// Enable the SPI1 module clock
	RCC->APBENR2|=RCC_APBENR2_SPI1EN;
	// Reset SPI1
	RCC->APBRSTR2|=RCC_APBRSTR2_SPI1RST;
	RCC->APBRSTR2&=~RCC_APBRSTR2_SPI1RST;
#elif defined(PN_GMR)
	// Set PA15 to high speed GPIO output (SPI CS)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (1<<(15*2));
	GPIOA->OSPEEDR=(GPIOA->OSPEEDR&~(3UL<<(15*2))) | (3UL<<(15*2));
	// Set PB3, PB4 and PB5 to high speed alternate SPI SCK, SPI MISO and SPI MOSI mode
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~((0xFUL<<(3*4)) | (0xF<<(4*4)) | (0xF<<(5*4)))) | (0<<(3*4)) | (0<<(4*4)) | (0<<(5*4));
	GPIOB->OSPEEDR=(GPIOB->OSPEEDR&~((3UL<<(3*2)) | (3<<(4*2)) | (3<<(5*2)))) | (3<<(3*2)) | (3<<(4*2)) | (3<<(5*2));
	GPIOB->MODER=(GPIOB->MODER&~((3UL<<(3*2)) | (3<<(4*2)) | (3<<(5*2)))) | (2<<(3*2)) | (2<<(4*2)) | (2<<(5*2));
	// Enable the SPI1 module clock
	RCC->APB2ENR|=RCC_APB2ENR_SPI1EN;
	// Reset SPI1
	RCC->APB2RSTR|=RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_SPI1RST;
#endif
	// CPHA=0 Clock phase
	// CPOL=0 Clock polarity
	// MSTR=1 Master mode
	// BR=1 SPI clock=SYSCLK/4=16MHz
	// SPE=0 SPI disabled
	// LSBFIRST=0 MSB first
	// SSI=1 Internal Slave Select at high level
	// SSM=1 Software slave management enabled
	// RXONLY=0 Full duplex
	// CRCL=0 Not used
	// CRCNEXT=0 Not used
	// CRCEN=0 CRC calculation disabled
	// BIDIOE=0 Not used
	// BIDIMODE=0 2-line unidirectional data mode selected
	SPIX->CR1=SPI_CR1_MSTR | (SPI_CR1_BR_0*1) | SPI_CR1_SSI | SPI_CR1_SSM;
	// RXDMAEN=0 RX DMA disabled
	// TXDMAEN=0 TX DMA disabled
	// SSOE=1 SS output enabled
	// NNSP=0 No NSS pulse
	// FRF=0 SPI Motorola mode
	// ERRIE=0 Error interrupt masked
	// RXNEIE=0 RXNE interrupt masked
	// TXEIE=0 TXE interrupt masked
	// DS=7 Data length is 8 bits
	// FRXTH=1 RXNE event on a 8 bits data
	// LDMA_RX=0 Not used
	// LDMA_TX=0 Not used
	SPIX->CR2=SPI_CR2_INIT_VALUE;
	// Enable SPI
	SPIX->CR1|=SPI_CR1_SPE;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// CS=1
	GPIOB->BSRR=1<<12;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// CS=1
	GPIOA->BSRR=1<<4;
#elif defined(PN_GMR)
	// CS=1
	GPIOA->BSRR=1<<15;
#endif
	// Permanent DMA setup
	// SPIX_TX
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// DMAREQ_ID=x Select the request source peripheral
	// SOIE=0 Synchronization overrun interrupt disabled
	// EGE=0 Event generation disabled
	// SE=0 Synchronization disabled
	// SPOL=0 Not used
	// NBREQ=0 Not used
	// SYNC_ID=0 Not used
	DMAMUX1[4].CCR=19<<DMAMUX_CxCR_DMAREQ_ID_Pos;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// DMAREQ_ID=x Select the request source peripheral
	// SOIE=0 Synchronization overrun interrupt disabled
	// EGE=0 Event generation disabled
	// SE=0 Synchronization disabled
	// SPOL=0 Not used
	// NBREQ=0 Not used
	// SYNC_ID=0 Not used
	DMAMUX1[4].CCR=17<<DMAMUX_CxCR_DMAREQ_ID_Pos;
#endif
	SPI_DMA_CHANNEL_TX->CPAR=(unsigned long)&SPIX->DR;
	// SPIX_RX
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// DMAREQ_ID=x Select the request source peripheral
	// SOIE=0 Synchronization overrun interrupt disabled
	// EGE=0 Event generation disabled
	// SE=0 Synchronization disabled
	// SPOL=0 Not used
	// NBREQ=0 Not used
	// SYNC_ID=0 Not used
	DMAMUX1[5].CCR=18<<DMAMUX_CxCR_DMAREQ_ID_Pos;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// DMAREQ_ID=x Select the request source peripheral
	// SOIE=0 Synchronization overrun interrupt disabled
	// EGE=0 Event generation disabled
	// SE=0 Synchronization disabled
	// SPOL=0 Not used
	// NBREQ=0 Not used
	// SYNC_ID=0 Not used
	DMAMUX1[5].CCR=16<<DMAMUX_CxCR_DMAREQ_ID_Pos;
#endif
	SPI_DMA_CHANNEL_RX->CPAR=(unsigned long)&SPIX->DR;
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// Set the DMA1_Ch4_7_DMAMUX1_OVR IRQ to high priority
	NVIC_SetPriority(DMA1_Ch4_7_DMAMUX1_OVR_IRQn,IRQ_PRI_REALTIME);
	// Enable the DMA1_Ch4_7_DMAMUX1_OVR IRQ
	NVIC_EnableIRQ(DMA1_Ch4_7_DMAMUX1_OVR_IRQn);
#elif defined(PN_GMR)
	// Set the DMA1_Channel2_3 IRQ to high priority
	NVIC_SetPriority(DMA1_Channel2_3_IRQn,IRQ_PRI_REALTIME);
	// Enable the DMA1_Channel2_3 IRQ
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
#endif
	pSPI_BusyIRQCallback=NULL;
	return;
}

// pRXData=NULL if no reception is needed
// pRXData can be equal to pTXData
void SPI_Transfer(const void *pTXData,void *pRXData,unsigned long DataLength,tPHY_BusyIRQCallback *pCallback)
{
	__DSB();
	pSPI_BusyIRQCallback=NULL;
	// Wait for Busy to be low
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	while (GPIOB->IDR&(1<<14));
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	while (GPIOA->IDR&(1<<12));
#elif defined(PN_GMR)
	while (GPIOB->IDR&(1<<0));
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// CS=1
	GPIOB->BSRR=1<<(16+12);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// CS=1
	GPIOA->BSRR=1<<(16+4);
#elif defined(PN_GMR)
	// CS=1
	GPIOA->BSRR=1U<<(16+15);
#endif
	// Set up the TX DMA
	SPI_DMA_CHANNEL_TX->CMAR=(unsigned long)pTXData;
	SPI_DMA_CHANNEL_TX->CNDTR=DataLength;
	// TX and RX
	if (pRXData)
	{
		SPI_IsTXAndRXDMA=TRUE;
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
		SPI_DMA_CHANNEL_TX->CCR=             DMA_CCR_DIR | DMA_CCR_MINC;
		SPI_DMA_CHANNEL_TX->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
		// Set up the RX DMA
		SPI_DMA_CHANNEL_RX->CMAR=(unsigned long)pRXData;
		SPI_DMA_CHANNEL_RX->CNDTR=DataLength;
		// EN=1 DMA channel enabled
		// TCIE=1 Transfer complete interrupt enabled
		// HTIE=0 Half transfer interrupt disabled
		// TEIE=0 Transfer error interrupt disabled
		// DIR=0 Read from peripheral
		// CIRC=0 Circular mode disabled
		// PINC=0 Peripheral increment mode disabled
		// MINC=1 Memory increment mode enabled
		// PSIZE=0 8-bit peripheral size
		// MSIZE=0 8-bit memory size
		// PL=0 Low priority
		// MEM2MEM=0 Memory to memory mode disabled
		SPI_DMA_CHANNEL_RX->CCR=             DMA_CCR_TCIE | DMA_CCR_MINC;
		SPI_DMA_CHANNEL_RX->CCR=DMA_CCR_EN | DMA_CCR_TCIE | DMA_CCR_MINC;
		// Clear the SX1280 Busy IRQ pending flag
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
		EXTI->FPR1=1<<14;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
		EXTI->FPR1=1<<12;
#elif defined(PN_GMR)
		EXTI->PR=1<<0;
#endif
		pSPI_BusyIRQCallback=pCallback;
		// Enable SPI RX and TX DMA
		SPIX->CR2=SPI_CR2_INIT_VALUE | SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;
	}
	// TX only
	else
	{
		SPI_IsTXAndRXDMA=FALSE;
		// EN=1 DMA channel enabled
		// TCIE=1 Transfer complete interrupt enabled
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
		SPI_DMA_CHANNEL_TX->CCR=             DMA_CCR_TCIE | DMA_CCR_DIR | DMA_CCR_MINC;
		SPI_DMA_CHANNEL_TX->CCR=DMA_CCR_EN | DMA_CCR_TCIE | DMA_CCR_DIR | DMA_CCR_MINC;
		// Clear the SX1280 Busy IRQ pending flag
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
		EXTI->FPR1=1<<14;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
		EXTI->FPR1=1<<12;
#elif defined(PN_GMR)
		EXTI->PR=1<<0;
#endif
		pSPI_BusyIRQCallback=pCallback;
		// Enable SPI TX DMA
		SPIX->CR2=SPI_CR2_INIT_VALUE | SPI_CR2_TXDMAEN;
	}
	return;
}

void SPI_TransferWait(const void *pTXData,void *pRXData,unsigned long DataLength)
{
	pSPI_BusyIRQCallback=NULL;
	// Wait for Busy to be low
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	while (GPIOB->IDR&(1<<14));
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	while (GPIOA->IDR&(1<<12));
#elif defined(PN_GMR)
	while (GPIOB->IDR&(1<<0));
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// CS=1
	GPIOB->BSRR=1<<(16+12);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// CS=1
	GPIOA->BSRR=1<<(16+4);
#elif defined(PN_GMR)
	// CS=1
	GPIOA->BSRR=1U<<(16+15);
#endif
	// Transmit only
	if (!pRXData)
	{
		while (DataLength)
		{
			while ((SPIX->SR & SPI_SR_FTLVL)==SPI_SR_FTLVL_0*3);
			*(volatile unsigned char *)&SPIX->DR=*(const unsigned char *)pTXData;
			pTXData=(const unsigned char *)pTXData+1;
			DataLength--;
		}
		// Wait for the end of the transfer
		while (SPIX->SR&SPI_SR_BSY);
		// Flush the RX buffer
		while (SPIX->SR&SPI_SR_RXNE)
			*(volatile unsigned char *)&SPIX->DR;
		// Clear the overrun flag
		SPIX->SR;
	}
	// Transmit and receive
	else
	{
		unsigned long RXDataLength;
		
		RXDataLength=DataLength;
		while (DataLength)
		{
			if ((SPIX->SR & SPI_SR_FTLVL)!=SPI_SR_FTLVL_0*3)
			{
				*(volatile unsigned char *)&SPIX->DR=*(const unsigned char *)pTXData;
				pTXData=(const unsigned char *)pTXData+1;
				DataLength--;
			}
			if ((SPIX->SR & SPI_SR_FRLVL)!=SPI_SR_FRLVL_0*0)
			{
				*(unsigned char *)pRXData=*(volatile unsigned char *)&SPIX->DR;
				pRXData=(unsigned char *)pRXData+1;
				RXDataLength--;
			}
		}
		while (RXDataLength)
		{
			while ((SPIX->SR & SPI_SR_FRLVL)==SPI_SR_FRLVL_0*0);
			*(unsigned char *)pRXData=*(volatile unsigned char *)&SPIX->DR;
			pRXData=(unsigned char *)pRXData+1;
			RXDataLength--;
		}
	}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// CS=1
	GPIOB->BSRR=1<<12;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	// CS=1
	GPIOA->BSRR=1<<4;
#elif defined(PN_GMR)
	// CS=1
	GPIOA->BSRR=1<<15;
#endif
	return;
}
