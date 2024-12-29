#if !defined(PHY_H)
#define PHY_H

#include "System definitions.h"
#include "PHY external.h"

#if defined(PHY_SX1280)
	#include "PHY SX1280.h"
#endif

#define PHY_DEFAULT_PERIOD  5000

#define PHY_MIN_TIMER_IRQ_TIME 50 // Minimum time between 2 timer IRQs

#define PHY_MAX_PACKET_SIZE (48)

#define PHY_RX_DURATION_INFINITE 0xFFFFFFFF
#define PHY_TX_DURATION_INFINITE 0xFFFFFFFF

#define PHY_DEFAULT_RF_POWER 0x7FFF

#define PHY_MIN_RSSI                 (0xFF*4) // In -0.25dBm unit
#define PHY_ADAPTIVE_RSSI            (70*4)  // In -0.25dBm unit

typedef void tPHY_TimedCallback(void);
typedef void tPHY_PacketReceivedCallback(const void *pPacket);

typedef enum
{
	PHY_SX1280_AMT_CCA, // Perform a normal Clear Channel Assessment and transmit only if the channel is clear
	PHY_SX1280_AMT_CCA_TX, // Perform a normal Clear Channel Assessment and always transmit
	PHY_SX1280_AMT_EXTENDED_CCA // Perform an extended CCA but do not transmit
} ePHY_AdaptiveModeType;

typedef enum
{
	PHY_PAM_IDLE,
	PHY_PAM_TX,
	PHY_PAM_RX
} ePHY_PAMode;

typedef struct
{
	unsigned long PreTX; // Time between the last timer IRQ and the real RF transmission
	unsigned long TX;  // Time of RF transmission
	unsigned long PostTX; // Time between the end of transmission and the next timer IRQ, can be zero
	unsigned long DelayPostTX; // Time between the last timer TX IRQ and the time when the host application gets the effective control
	unsigned long PreRX; // Time between the last timer IRQ and the real RF reception
	unsigned long RX; // Time of RF reception
	unsigned long PostRX; // Time between the end of reception and the next timer IRQ, can be zero
	unsigned long DelayPostRX; // Time between the last timer RX IRQ and the time when the host application gets the effective control
	unsigned long NbChannels; // Number of usable non-overlapping channels, numbered from 0 to NbChannels-1
	unsigned long Bandwidth; // In units of 100Hz, should be even
	signed long MinSNR; // Minimum SNR at which a packet may be received in unit of -0.25dB
} sPHY_Timings;

typedef struct __attribute__((packed))
{
	signed short TXPowerCorrection; // RF TX power correction in 0.25dB unit
	signed char RSSICorrection; // Local RSSI correction in 0.25dB unit
	signed char FrequencyOffsetCorrection; // Local frequency offset correction in 1Khz unit
} sPHY_FactoryConfig;

typedef struct
{
	const sPHY_FactoryConfig *pFactoryConfig;
	signed long DefaultRFPower; // RF power used when RFPower is set to PHY_DEFAULT_RF_POWER
	signed long PATXGain; // Global gain (or loss) between the RF chip output and the antenna(s) when transmitting in 0.25dB units
	signed long PARXGain; // Global gain (or loss) between the antenna(s) and the RF chip output when receiving in 0.25dB units
	unsigned long Attenuator; // Switchable attenuator attenuation in 0.25dB unit
} sPHY_Init;

typedef struct
{
	tPHY_TimedCallback *pTimedTXCallback; // NULL if no callback, usable only if TXDuration!=PHY_TX_DURATION_INFINITE
	unsigned long TimedTXCallbackTime; // In microseconds, valid only if pTXCallback!=NULL
	unsigned long TXDuration; // Valid only in carrier only mode
	unsigned long AdditionalPreTXTime;
	unsigned long AdditionalPostTXTime;
	signed long ChannelNb; // If negative, represents the positive frequency offset from the beginning of the band in 100Hz increments
	signed long RFPower; // In units of 0.25dBm
	unsigned long AdaptiveRSSI; // Measured RF power when executing the CCA in -0.25dBm unit
	unsigned long AdaptiveRSSIThreshold; // CCA RF power threshold in -0.25dBm unit (inclusive)
	BOOL PAOn;
	unsigned char AntennaNb; // 0 or 1
	ePHY_AdaptiveModeType AdaptiveModeType; // Valid only if sPHY_Config.AdaptiveMode==TRUE
	BOOL ChannelClear; // Valid only if sPHY_Config.AdaptiveMode==TRUE
} sPHY_TXConfig;

typedef struct
{
	tPHY_PacketReceivedCallback *pPacketReceivedCallback;
	tPHY_TimedCallback *pTimedRXCallback; // NULL if no callback
	unsigned long RXDuration;
	signed long TimedRXCallbackTime; // In microseconds, valid only if pRXCallback!=NULL, periodic if negative
	unsigned long AdditionalPreRXTime;
	unsigned long AdditionalPostRXTime;
	signed long ChannelNb; // If negative, represents the positive frequency offset from the beginning of the band in 100Hz increments
	BOOL PAOn;
	unsigned char AntennaNb; // 0 or 1
	BOOL ResumeRXOnFailure;
	unsigned char Padding;
} sPHY_RXConfig;

void RFM_AddSpectrumAnalyzerValue(unsigned char ChannelNb,unsigned long RSSI);
void PHY_SetAntenna(unsigned char AntennaNb);
void PHY_SetAttenuator(BOOL AttenuatorOn);
void PHY_SetPAMode(ePHY_PAMode PAMode);

unsigned long PHY_GetInstantRSSI(void);
void PHY_GetLastRXConditions(sPHY_RXConditions *pRXConditions);
void PHY_Init(const sPHY_Init *pInit);
BOOL PHY_IsChannelUsable(unsigned long ChannelNb,unsigned long RFBandLow,unsigned long RFBandHigh);
void PHY_ResumeRX(void);
void PHY_RX(sPHY_RXConfig *pRXConfig);
void PHY_SetConfig(const sPHY_Config *pConfig,sPHY_Timings *pTimings,BOOL CalculateTimingsOnly);
#if defined(ROLE_TX)
	unsigned long PHY_SetSpectrumAnalyzerConfig(sPHY_SpectrumAnalyzerConfig *pConfig);
	void PHY_SpectrumAnalyzer(void);
#endif
void PHY_Stop(tPHY_BusyIRQCallback *pCallback);
void PHY_TX(const void *pPacket,sPHY_TXConfig *pTXConfig);

BOOL PHY_TimerCompareIRQHandler(void);

void LNKTIM_SetNextCompareIRQTime(unsigned long Time);
void LNKTIM_SetNextCompareIRQTimeFromMarker(unsigned long Time);
void LNKTIM_SetTimeMarker(void);
void LNKTIM_StartTimer(unsigned long FirstCompareIRQTime);
void LNKTIM_StopTimer(void);

#endif // !defined(PHY_H)
