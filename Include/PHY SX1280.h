#if !defined(PHY_SX1280_H)
#define PHY_SX1280_H

#include "System definitions.h"

#include "PHY SX1280 hardware.h"

typedef void tPHY_BusyIRQCallback(void);
	
typedef struct __attribute__((packed))
{
	unsigned char PhyTXPayloadLength;
	unsigned char PhyRXPayloadLength;
	BOOL CarrierOnly:1;
	BOOL MeasureRSSI:1; // Measure RSSI if TRUE
	BOOL AdaptiveMode:1; // Adaptive frequency hopping mode
	BOOL Reserved:5;
	ePHY_SX1280_PacketType PacketType;
	sPHY_SX1280_PacketParams PacketParams;
	sPHY_SX1280_ModulationParams ModulationParams;
} sPHY_Config;

#if defined(ROLE_TX)
typedef struct
{
	unsigned long ChannelNb; // Channel being measured
	unsigned long RSSI; // RSSI of the channel being measured, valid only in the callback
	ePHY_SX1280_FLRC_BitrateBandwidth Bandwidth;
	unsigned char AntennaNb; // 0 or 1
	unsigned char Padding[2];
} sPHY_SpectrumAnalyzerConfig;
#endif

void PHY_SX1280_DIO1IRQHandler(void);
BOOL PHY_SX1280_IsBusy(void);
void PHY_SX1280_SetResetPin(BOOL Level);

void SPI_Transfer(const void *pTXData,void *pRXData,unsigned long DataLength,tPHY_BusyIRQCallback *pCallback);
void SPI_TransferWait(const void *pTXData,void *pRXData,unsigned long DataLength);

void SYS_DelayUs(unsigned long Delay_us);

#endif // !defined(PHY_SX1280_H)
