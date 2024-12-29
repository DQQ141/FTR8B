#if !defined(SPI_SX1280_H)
#define SPI_SX1280_H

#include "System definitions.h"

#include "PHY SX1280.h"

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void);
#elif defined(PN_GMR)
	void DMA1_Channel2_3_IRQHandler(void);
#endif
void SPI_SX1280_BusyIRQHandler(void);
void SPI_Init(void);
void SPI_Transfer(const void *pTXData,void *pRXData,unsigned long DataLength,tPHY_BusyIRQCallback *pCallback);
void SPI_TransferWait(const void *pTXData,void *pRXData,unsigned long DataLength);

#endif // defined(SPI_SX1280_H)
