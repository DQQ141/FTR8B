#if !defined(LNK_AFHDS3_H)
#define LNK_AFHDS3_H

#include "System definitions.h"
#if defined(ROLE_TX)
	#include "Update.h"
#endif
#include "PHY.h"

#if defined(ROLE_RX)
	#include "LNK AFHDS3 receiver external.h"
#endif

#include "SES AFHDS3 external.h"

// Company codes
#define LNK_CC_ALL    0x0000
#define LNK_CC_FLYSKY 0x0001

#define LNK_NB_HOPS                         32

#define LNK_MAX_PAYLOAD_LENGTH              36

typedef union tLNK_BoundConfig uLNK_BoundConfig;
typedef struct tLNK_BlackBoxData sLNK_BlackBoxData;
typedef void tLNK_BindCallback(void);
#if defined(ROLE_TX)
	typedef void tLNK_PacketReceivedCallback(const void *pPacket,BOOL IsAuxiliaryReceiver);
#elif defined(ROLE_RX)
	typedef void tLNK_PacketReceivedCallback(const void *pPacket);
#endif
typedef void tLNK_BuildTXPacketCallback(void *pPacket);
#if defined(ROLE_TX)
	typedef void tLNK_SpectrumAnalyzerCallback(void);
	typedef void tLNK_UpdatePacketReceivedCallback(const sLNK_UpdateResponse *pPacket);
	typedef void tLNK_UpdateBuildTXPacketCallback(sLNK_UpdateCommand *pPacket);
#endif
typedef void tLNK_FactorySaveConfigCallback(void);
typedef void tLNK_HeartbeatCallback(void);
typedef void tLNK_BlackBoxCallback(const sLNK_BlackBoxData *pData);

typedef enum
{
	LNK_ANTENNA_BOTH,
	LNK_ANTENNA_1,
	LNK_ANTENNA_2,
} eLNK_Antenna;

typedef enum
{
	LNK_ES_FREE,
	LNK_ES_CE,
	LNK_ES_FCC
} eLNK_EMIStandard;

typedef enum
{
	LNK_RR_RF_RECEIVER,
	LNK_RR_I_BUS_1_PWM_ADAPTER,
	LNK_RR_I_BUS_2_PWM_ADAPTER,
	LNK_RR_FACTORY,
	LNK_RR_FACTORY_NO_CALIBRATION
} eLNK_ReceiverRole;

typedef struct tLNK_BlackBoxData
{
	const void *pDataPacket;
	unsigned long DataPacketLength;
	signed long ChannelNb; // If negative, represents the positive frequency offset from the beginning of the band in 100Hz increments
	signed long RFPower; // In units of 0.25dBm
	unsigned long Duration;
	BOOL IsTX; // FALSE: RX, TRUE: TX
	BOOL PAOn;
	unsigned char AntennaNb; // 0 or 1
	unsigned char Padding;
} sLNK_BlackBoxData;

typedef struct __attribute__((packed))
{
	unsigned long AuxiliaryTime; // Time inserted after each feedback for an auxiliary function, may be zero
	unsigned short SyncTimeout; // In milliseconds
	unsigned short HopSequence[LNK_NB_HOPS];
	signed short RFPower; // RF power used when running in units of 0.25dBm
	BOOL PAOn;
	eLNK_Antenna Antenna;
	sPHY_Config PHYConfig;
	unsigned char RXTimeMargin; // Time margin before and after reception for both transmitter and receiver in units of 10us, cannot be zero
} sLNK_RFBand;

typedef struct
{
	eLNK_ReceiverRole ReceiverRole;
	unsigned char Version; // =0
	unsigned char TXPayloadLength; // Length of transmitter payload
	unsigned char RXPayloadLength; // Length of feedback (receiver) payload
#if defined(ROLE_TX)
	unsigned long RXID;
#elif defined(ROLE_RX)
	unsigned long TXID;
#endif
	unsigned char FeedbackRatio; // 0=1-way transmitter, 1=feedback every cycle, 2=feedback every 2 cycles... up to 15
	sLNK_RFBand RFBand;
	eLNK_EMIStandard EMIStandard;
	BOOL BroadcastRXID; // If FALSE, the RX ID is not broadcasted to save 4 bytes on each side
} sLNK_BoundConfigRFCommon;

#if AFHDS3_VERSION==0 || defined(ROLE_TX)
typedef struct
{
	sLNK_BoundConfigRFCommon Common;
} sLNK_BoundConfigRFV0;
#endif

#if AFHDS3_VERSION==1
typedef struct
{
	sLNK_BoundConfigRFCommon Common;
#if defined(ROLE_TX)
	unsigned long AuxiliaryRXID;
#endif
#if defined(LNK_DUAL_RF_BAND)
	sLNK_RFBand RFBandBackup;
#endif
	BOOL Has2Receivers;
#if defined(ROLE_TX)
	BOOL IsAuxiliaryReceiver; // Indicates the receiver to bind to, ignored when in run mode
#elif defined(ROLE_RX)
	BOOL IsAuxiliaryReceiver; // Indicates the role of the receiver, main or auxiliary
#endif
#if defined(LNK_DUAL_RF_BAND)
	unsigned char Padding;
#else
	unsigned char Padding[2];
#endif
} sLNK_BoundConfigRFV1;
#endif

typedef struct
{
	eLNK_ReceiverRole ReceiverRole;
	BOOL PAOn;
	unsigned char Padding[2];
	signed long RFPower; // In units of 0.25dBm
} sLNK_BoundConfigFactory;

typedef struct
{
	eLNK_ReceiverRole ReceiverRole;
	sSES_PWMFrequenciesHalfV1 PWMFrequencies[2]; // For channels 1 to 16 and 17 to 32
	unsigned char PWMChannelsNb[SES_NB_MAX_CHANNELS];
} sLNK_BoundConfigPWMAdapter;

typedef union tLNK_BoundConfig
{
	eLNK_ReceiverRole ReceiverRole;
	sLNK_BoundConfigRFCommon RFCommon;
#if AFHDS3_VERSION==0 || defined(ROLE_TX)
	sLNK_BoundConfigRFV0 RFV0;
#endif
#if AFHDS3_VERSION==1
	sLNK_BoundConfigRFV1 RFV1;
#endif
	sLNK_BoundConfigFactory Factory;
	sLNK_BoundConfigPWMAdapter PWMAdapter;
} uLNK_BoundConfig;

typedef struct
{
	// Bind packet header
	unsigned char ConfigLength;
	unsigned char ConfigVersion; // =0 or 1
} sLNK_BoundConfig1WayHeader;

#if AFHDS3_VERSION==0 || defined(ROLE_TX)
typedef struct
{
	// Bind packet header
	unsigned char ConfigLength;
	unsigned char ConfigVersion; // =0
	// Transport
	unsigned char TransportReserved;
	// Session
	unsigned char ChannelsType[SES_CHANNELS_TYPE_LENGTH]; // 5 bits per channel from the values defined in SES_CT_*
	unsigned char Failsafe[SES_MAX_CHANNELS_LENGTH];
	unsigned char SignalStrengthRCChannelNb; // 0xFF if not used
	unsigned short FailsafeTimeout;
	// Application
	unsigned char AnalogOutput; // eSES_PA_SetAnalogOutput type
	unsigned char ExternalBusType; // eEB_BusType type
	sSES_PWMFrequencyV0 PWMFrequency;
	unsigned char IsFailsafeInNoOutputMode;
	unsigned char Padding;
} sLNK_BoundConfig1WayV0;
#endif

#if AFHDS3_VERSION==1
typedef struct
{
	// Bind packet header
	unsigned char ConfigLength;
	unsigned char ConfigVersion; // =1
	unsigned char ConfigLengthHigh;
	// Transport
	unsigned char TransportReserved;
	// Session
	unsigned short FailsafeTimeout;
	unsigned char ChannelsType[SES_CHANNELS_TYPE_LENGTH]; // 5 bits per channel from the values defined in SES_CT_*
	unsigned char Failsafe[SES_MAX_CHANNELS_LENGTH];
	unsigned char SignalStrengthRCChannelNb; // 0xFF if not used
	// Application
	unsigned char RXFirstPWMRCChannelNb; // Number of the RC channel to output on the first PWM output of the receiver
	unsigned char IsFailsafeInNoOutputMode:1;
	unsigned char Reserved:7;
	sSES_PWMFrequenciesHalfV1 PWMFrequencies[2]; // For channels 1 to 16 and 17 to 32
	eSES_NewPortType NewPortTypes[SES_NPT_NB_MAX_PORTS];
	unsigned char Padding;
} sLNK_BoundConfig1WayV1;
#endif

typedef union
{
	sLNK_BoundConfig1WayHeader Header;
#if AFHDS3_VERSION==0 || defined(ROLE_TX)
	sLNK_BoundConfig1WayV0 V0;
#endif
#if AFHDS3_VERSION==1
	sLNK_BoundConfig1WayV1 V1;
#endif
} uLNK_BoundConfig1Way;

// Structure supplied by the application
typedef struct
{
	uLNK_BoundConfig1Way *pBoundConfig1Way;
#if defined(ROLE_TX)
	signed long BindRFPower; // Power used when binding in unit of 0.25dBm
	BOOL BindPAOn; // PA on or off  when binding
	BOOL UseFirstAndLastRFChannels;
	unsigned char Padding[2];
#endif
	unsigned long RFBandLow; // Lowest RF frequency allowed by the application counted from the base frequency in 100Hz increments, 0 if no limit
	unsigned long RFBandHigh; // Highest RF frequency allowed by the application counted from the base frequency in 100Hz increments, 0 if no limit
} sLNK_BindApp;

// Structure supplied by the upper layer (transport)
typedef struct
{
	tLNK_BindCallback *pBindCallback;
	uLNK_BoundConfig *pBoundConfig;
} sLNK_BindUp;

typedef struct __attribute__((packed))
{
	unsigned long Length;
	unsigned short InternalBatteryMonitorCorrection; // Internal battery monitor ADC correction, 1<<15=1.0
#if AFHDS3_VERSION==1
	unsigned short ExternalBatteryMonitorCorrection; // External battery monitor ADC correction, 1<<15=1.0
#endif
	sPHY_FactoryConfig PHY;
	unsigned long CRC32;
} sLNK_FactoryConfig;

// Structure supplied by the application
typedef struct
{
	tLNK_FactorySaveConfigCallback *pFactorySaveConfigCallback;
	signed short RFPower; // RF power used when in factory mode in unit of 0.25dBm
	BOOL PAOn;
	BOOL HasTXOnly;
	BOOL HasRXOnly;
	BOOL NoCalibration;
	unsigned char Padding[2];
} sLNK_FactoryApp;

typedef struct
{
	sLNK_FactoryConfig *pFactoryConfig;
	unsigned long LocalID; // Unique 32-bit ID
	unsigned short CompanyCode; // Will bind only to receivers of this company
	unsigned char Padding[2];
	sPHY_Init PhyInit;
} sLNK_Init;

// Structure supplied by the application
typedef struct
{
	tLNK_BlackBoxCallback *pBlackBoxCallback;
	unsigned long BuildTXPacketTime; // Time between the call of pBuildTXPacketCallback and the end of the RF transaction
} sLNK_RunApp;

// Structure supplied by the upper layer (transport)
typedef struct
{
	tLNK_PacketReceivedCallback *pPacketReceivedCallback;
	tLNK_BuildTXPacketCallback *pBuildTXPacketCallback;
	tLNK_HeartbeatCallback *pHeartbeatCallback;
	uLNK_BoundConfig *pBoundConfig;
} sLNK_RunUp;

#if defined(ROLE_TX)
typedef struct
{
	tLNK_SpectrumAnalyzerCallback *pSpectrumAnalyzerCallback;
	unsigned long FirstChannelsNb;
	unsigned long LastChannelsNb;
	sPHY_SpectrumAnalyzerConfig PhysicalConfig;
} sLNK_SpectrumAnalyzerApp;
#endif

typedef enum
{
	LNK_TT_CARRIER_ONLY,
	LNK_TT_SINGLE_CHANNEL,
	LNK_TT_WIDE_HOPPING,
	LNK_TT_FREQUENCY_SCAN
} eLNK_TestType;

typedef struct __attribute__((packed))
{
	signed long ChannelNb; // If negative, represents the positive frequency offset from the beginning of the band in 100Hz increments
	unsigned short AuxiliaryTime; // Time inserted after each feedback for an auxiliary function, may be zero
	signed short RFPower; // Power used when testing in unit of 0.25dBm
	eLNK_TestType Type:4;
	eLNK_Antenna Antenna:2;
	BOOL PAOn:1;
	BOOL BroadcastRXID:1; // If FALSE, the RX ID is not broadcasted to save 4 bytes on each side
	unsigned char TestTXPayloadLength; // Length of transmitter payload
	unsigned char TestRXPayloadLength; // Length of feedback (receiver) payload
	unsigned char FeedbackRatio; // 0=1-way transmitter, 1=feedback every cycle, 2=feedback every 2 cycles... up to 15
	sPHY_Config PHYConfig; // RF configuration used when testing
	unsigned char RXTimeMargin; // Time margin before and after reception for both transmitter and receiver in units of 10us
	eLNK_EMIStandard EMIStandard;
} sLNK_Test;

#if defined(ROLE_TX)
typedef struct
{
	tLNK_UpdateBuildTXPacketCallback *pBuildTXPacketCallback;
	tLNK_UpdatePacketReceivedCallback *pPacketReceivedCallback;
} sLNK_UpdateApp;
#endif

typedef void tLNKTIM_TimerCompareIRQHandler(void);

unsigned long ADC_GetInternalVoltage(void);
#if AFHDS3_VERSION==1
	unsigned long ADC_GetExternalVoltage(void);
#endif

void LNKTIM_Init(tLNKTIM_TimerCompareIRQHandler *pTimerCompareIRQHandler);
void LNKTIM_SetNextCompareIRQTime(unsigned long Time);
void LNKTIM_SetNextCompareIRQTimeFromMarker(unsigned long Time);
void LNKTIM_SetTimeMarker(void);
void LNKTIM_StartTimer(unsigned long FirstCompareIRQTime);
void LNKTIM_StopTimer(void);

void LNK_Bind(const sLNK_BindApp *pBindApp,const sLNK_BindUp *pBindUp);
void LNK_Factory(const sLNK_FactoryApp *pFactoryApp);
void LNK_Idle(void);
signed short LNK_GetRFPower(void);
void LNK_Init(const sLNK_Init *pInit);
unsigned long LNK_Run(const sLNK_RunApp *pRunApp,const sLNK_RunUp *pRunUp);
void LNK_SetAntenna(eLNK_Antenna Antenna);
void LNK_SetRFPower(signed short Power);
#if defined(ROLE_TX)
	unsigned long LNK_SpectrumAnalyzer(sLNK_SpectrumAnalyzerApp *pSpectrumAnalyzerApp);
#endif
void LNK_Stop(void);
void LNK_Test(sLNK_Test *pTest);
#if defined(ROLE_TX)
	void LNK_Update(const sLNK_UpdateApp *pUpdateApp);
#endif

#endif // !defined(LNK_AFHDS3_H)
