#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif
#include <string.h>

#include "System definitions.h"
#include "System.h"

#include "AFHDS3 CRC library.h"
#include "AFHDS3 PWM frequencies packing library.h"
#include "AFHDS3 queue low priority IRQ library.h"
#include "AFHDS3 RC channels packing library.h"
#include "System receiver.h"

#include "ADC.h"
#include "EEPROM.h"
#include "I2C.h"
#include "i-Bus.h"
#include "i-Bus 1.h"
#include "i-Bus 2.h"
#if defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_FBR8) 
	#include "i-Bus 1 device library.h"
	#include "i-Bus 2 device library.h"
#endif
#include "LED.h"
#include "Main.h"
#include "New Port.h"
#include "NPT timer.h"
#include "PWM adapter.h"
#include "SES AFHDS3.h"
#include "SPI SX1280.h"
#include "USART.h"
#if defined(PN_INR6_HS)  
	#include "Bmp280.h"
#endif

#if defined(PN_INR4_GYB) 
  #include "Gyro.h"
#elif defined(PN_GMR)
	#include "GyroGMr.h"
#endif

#if defined(PN_INR6_FC)
	#include "FC.h"
#endif

#if !defined(DEBUG)	
	#include "Encryption.h"
	#include "Flash.h"
#endif	


#include "Main private.h"

#define APP_ConfigRF APP_Config.C.RF
#define APP_ConfigPWM APP_Config.C.PWM

#if defined(DEBUG)
	#define LNK_RXID 0x12345679
	#define LNK_CompanyCode LNK_CC_ALL
#endif

#define TRA_WS_TX_BUFFER_SIZE 256
#define NPT_WS_TX_BUFFER_SIZE 256
#define NPT_WS_RX_BUFFER_SIZE 256
#define APP_WS_DEFAULT_USART_SPEED 115200

static volatile BOOL APP_Binding;
static unsigned char APP_BindCommandNb;
static unsigned long APP_TestMs;
static sSES_Test SES_TestStruct;
static unsigned char TRA_WSTXBuffer[TRA_WS_TX_BUFFER_SIZE];
static unsigned char NPT_WSTXBuffer[NPT_WS_TX_BUFFER_SIZE];
static unsigned char NPT_WSRXBuffer[NPT_WS_RX_BUFFER_SIZE];

// FALSE=APP_Channels1, TRUE=APP_Channels2 in use
// APP_NPT_InitStruct.pChannels and APP_NPT_InitStruct.pChannelsPacked point the channels in use
// SES_RunStruct.pChannels and SES_RunStruct.pChannelsPacked point to the other set of channels
static BOOL APP_SecondChannelsSetActive;
// RF receiver mode
//     Set 1 and 2 are used altervatively
// PWM adapter mode
//     APP_ChannelsPacked1 and APP_Channels1 are the channels received from the i-Bus 2 bus
//     APP_Channels2 contains the reordered RC channels according to the PWM channels configuration
//     APP_Channels2 is sent to the New Port PWM driver
unsigned char APP_ChannelsPacked1[SES_MAX_CHANNELS_LENGTH];
static unsigned char APP_ChannelsPacked2[SES_MAX_CHANNELS_LENGTH];
signed short APP_Channels1[SES_NB_MAX_CHANNELS];
signed short APP_Channels2[SES_NB_MAX_CHANNELS];
signed short APP_UsedFailsafe[SES_NB_MAX_CHANNELS];
unsigned char APP_UsedFailsafePacked[SES_MAX_CHANNELS_LENGTH];
sSES_PWMFrequenciesAPPV1 APP_PWMFrequenciesV1;
static unsigned long APP_SetChannelsCallbackTimeMs;
static unsigned long APP_SetChannelsMaxTime;
eSES_SignalStatus APP_SignalStatus;
sAPP_Config APP_Config;
BOOL APP_ConfigChanged;
static eSES_NewPortType APP_NewPortTypes[SES_NPT_NB_MAX_PORTS]; // Used to store the future configuration when deinitialization is in progress
static volatile BOOL APP_SwitchNewPortInProgress;
static BOOL APP_ChannelsReceived;
static eLNK_ReceiverRole APP_ActivateReceiverRole;
static BOOL APP_UpdateFirmware;
static unsigned long APP_BindSwitchPressedMs;

static sSES_Command APP_SESCommand;

#if defined(PN_TR8B)
static unsigned long APP_SysTickMs;
eAPP_ModeStatus   APP_ModeStatus;
sAPP_Config APP_OldConfig;
#endif
#if defined(PN_INR4_GYB)|| defined(PN_GMR)
static sSES_CA_SVC APP_DefaultSVC={
	.SteeringGain=100,
	.ThrottleGain=100,
	.Priority=50,
	.Neutral=50,
	.EndpointLow=100,
	.EndpointHigh=100,
	.GyroReverse=FALSE,
	.ThrottleReverse=FALSE,
	.ESPMode=0,
	.Reserved=0
};

static sSES_CA_ADVANCED_SVC APP_DefaultAdvancedSVC={
//    .TurningGain=50,        
//    .ForwardSpeed=50,               
//    .ReturnSpeed=50,            
//    .Filter=50,        
//    .CarType=1,                 
    .TurningGain=0,        
    .ForwardSpeed=0,               
    .ReturnSpeed=0,            
    .Filter=0,        
    .CarType=0,       
};
#endif

static sNPT_Init APP_NPT_InitStructRF={
		.pChannelsType=APP_Config.BoundConfig.ChannelsType,
		.pPWMFrequenciesAPP=&APP_PWMFrequenciesV1,
		.pChannelsPacked=APP_ChannelsPacked1,
		.pChannels=APP_Channels1,
		.pRXFirstPWMRCChannelNb=&APP_ConfigRF.RXFirstPWMRCChannelNb,
		.pFailsafePacked=APP_UsedFailsafePacked,
		.pFailsafe=APP_UsedFailsafe,
		.pIsFailsafeInNoOutputMode=&APP_ConfigRF.IsFailsafeInNoOutputMode,
		.pSignalStatus=&APP_SignalStatus,
		.pSupportedSensors=APP_ConfigRF.SupportedSensors.Types,
		.pWSUSARTConfig=&APP_ConfigRF.WSUSARTConfig,
		.pWSTXBuffer=NPT_WSTXBuffer,
		.pWSRXBuffer=NPT_WSRXBuffer,
		.WSTXBufferSize=NPT_WS_TX_BUFFER_SIZE,
		.WSRXBufferSize=NPT_WS_RX_BUFFER_SIZE,
		.RXPayloadLength=0,
		.pNewPortTypes=APP_Config.NewPortTypes,
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D) || defined(PN_FBR8)
		.NbInternalSensors=5+1   // IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS + external voltage
#elif defined(PN_INR4_GYB)
		.NbInternalSensors=5+1+1 // IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS + external voltage + 1-axis gyroscope
#elif defined(PN_INR6_HS)  
		.NbInternalSensors=5+1+1   // IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS + external voltage + pressure 
#elif defined(PN_TMR) || defined(PN_FBR12) || defined(PN_FBR4)
		.NbInternalSensors=5     // IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS 
#elif defined(PN_GMR)
		.NbInternalSensors=5+1   // IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS + 1-axis gyroscope
#endif
};

#if !defined(PN_TR8B)
static const unsigned char PWM_RXFirstPWMRCChannelNb=0;
static sNPT_Init APP_NPT_InitStructPWM={
		.pChannelsType=NULL,
		.pPWMFrequenciesAPP=&APP_PWMFrequenciesV1,
		.pChannelsPacked=APP_ChannelsPacked1,
		.pChannels=APP_Channels1,
		.pRXFirstPWMRCChannelNb=&PWM_RXFirstPWMRCChannelNb,
		.pFailsafePacked=APP_UsedFailsafePacked,
		.pFailsafe=APP_UsedFailsafe,
		.pIsFailsafeInNoOutputMode=NULL,
		.pSignalStatus=&APP_SignalStatus,
		.pSupportedSensors=NULL,
		.pWSUSARTConfig=NULL,
		.pWSTXBuffer=NULL,
		.pWSRXBuffer=NULL,
		.WSTXBufferSize=0,
		.WSRXBufferSize=0,
		.RXPayloadLength=0,
		.pNewPortTypes=APP_Config.NewPortTypes,
		.NbInternalSensors=0
};
#endif

sLNK_FactoryConfig LNK_FactoryConfig;
static BOOL LNK_FactoryConfigChanged;

static sSES_Init SES_InitStruct={
	.TransportInit={
		.LinkInit={
			.pFactoryConfig=&LNK_FactoryConfig,
			.LocalID=0,
			.CompanyCode=0,
			.PhyInit={
				.pFactoryConfig=&LNK_FactoryConfig.PHY,
#if defined(PN_FTR8B) || defined(PN_FTR12B) || defined(PN_FBR12)
				.DefaultRFPower=(signed long)(16*4),
				.PATXGain=(signed long)(18*4),
				.PARXGain=(signed long)(5.5*4),
				.Attenuator=0,
#elif defined(PN_FGR4B) || defined(PN_FGR8B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
				.DefaultRFPower=(signed long)(16*4),
				.PATXGain=(signed long)(22*4),
				.PARXGain=(signed long)(9.5*4),
				.Attenuator=0,
#elif defined(PN_INR6_HS) || defined(PN_GMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B)
				.DefaultRFPower=(signed long)(11*4),
				.PATXGain=(signed long)(-0.5*4),
				.PARXGain=(signed long)(-0.5*4),
				.Attenuator=0,
#elif defined(PN_TMR)
				.DefaultRFPower=(signed long)(11*4),
				.PATXGain=(signed long)(-0.5*4),
				.PARXGain=(signed long)(-0.5*4),
				.Attenuator=0,
#endif
			}
		}
	}
};

static sTRA_RunApp TRA_RunApp={
	.pWSTXBuffer=TRA_WSTXBuffer,
	.WSTXBufferSize=TRA_WS_TX_BUFFER_SIZE,
	.LinkRunApp={
		.pBlackBoxCallback=NULL,
		.BuildTXPacketTime=1000
	}
};

static sSES_Run SES_RunStruct={
	.pRealtimePacketReceivedCallback=SES_RealtimePacketReceivedCallback,
	.pWSDataReceivedCallback=SES_WSDataReceivedCallback,
	.pCommandReceivedCallback=SES_CommandReceivedCallback,
	.pPreSetChannelsCallback=SES_PreSetChannelsCallback,
	.pSetChannelsCallback=SES_SetChannelsCallback,
	.pBuildRealtimePacketCallback=SES_BuildRealtimePacketCallback,
	.pHeartbeatCallback=SES_HeartbeatCallback,
	.pBoundConfig=&APP_Config.BoundConfig,
	.pChannelsPacked=APP_ChannelsPacked2,
	.pChannels=APP_Channels2,
	.pUsedFailsafePacked=APP_UsedFailsafePacked,
	.pUsedFailsafe=APP_UsedFailsafe,
	.pTransportRunApp=&TRA_RunApp
};

static uLNK_BoundConfig1Way LNK_BoundConfig1Way;
static const sSES_Bind SES_BindStruct={
	.pBindCallback=SES_BindCallback,
	.pSessionRun=&SES_RunStruct,
	.TransportBindApp={
		.LinkBindApp={
			.pBoundConfig1Way=&LNK_BoundConfig1Way
		}
	}
};

static sSES_FactoryApp SES_FactoryApp={
	.TransportFactory={
		.LinkFactory={
			.pFactorySaveConfigCallback=LNK_FactorySaveConfigCallback,
			.RFPower=0,
			.HasTXOnly=FALSE,
			.HasRXOnly=FALSE,
			.NoCalibration=FALSE
		}
	}
};

static const sSES_CA_GetCapabilitiesResponseV1 SES_Capabilities={
	.NbRCChannels=NPT_NB_CHANNELS-1,
	.NbNewPortPorts=SES_NPT_NB_PORTS,
#if defined(PN_FTR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_INR6_HS) || defined(PN_TMR)|| defined(PN_TR8B)
	.HasTwoAntennas=TRUE,
	.HasSVC=FALSE,
	.HasRXFlightController=FALSE,
    .HasAdvancedSVC=FALSE,
	.Reserved1=0,
	.Reserved2={0}
#elif defined(PN_INR6_FC)
	.HasTwoAntennas=TRUE,
	.HasSVC=FALSE,
	.HasRXFlightController=TRUE,
    .HasAdvancedSVC=FALSE,
	.Reserved1=0,
	.Reserved2={0}
#elif defined(PN_FGR4D)
	.HasTwoAntennas=TRUE,
	.HasSVC=FALSE,
	.HasRXFlightController=FALSE,
    .HasAdvancedSVC=FALSE,
	.Reserved1=0,
	.Reserved2={0}
#elif defined(PN_FGR4B) || defined(PN_FGR8B) || defined(PN_FGR12B) || defined(PN_FBR8) || defined(PN_FBR4)
	.HasTwoAntennas=FALSE,
	.HasSVC=FALSE,
	.HasRXFlightController=FALSE,
    .HasAdvancedSVC=FALSE,
	.Reserved1=0,
	.Reserved2={0}
#elif defined(PN_INR4_GYB) 
	.HasTwoAntennas=FALSE,
	.HasSVC=TRUE,
	.HasRXFlightController=FALSE,
    .HasAdvancedSVC=TRUE,
	.Reserved1=0,
	.Reserved2={0}
#elif defined(PN_GMR)
    .HasTwoAntennas=FALSE,
    .HasSVC=TRUE,
    .HasRXFlightController=FALSE,
    .HasAdvancedSVC=FALSE,
    .Reserved1=0,
    .Reserved2={0}
#endif
};

static const sUSART_Config APP_WS_DefaultUSARTConfig={
	.BitDuration=USART_BAUDRATE_TO_BIT_DURATION(APP_WS_DEFAULT_USART_SPEED),
	.Parity=USART_PARITY_NONE,
	.StopBits=USART_STOPBIT_1,
	.Inverted=FALSE
};


#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
void EXTI4_15_IRQHandler(void)
{
	if (EXTI->FPR1&(1<<14))
	{
		EXTI->FPR1=1<<14;
		SPI_SX1280_BusyIRQHandler();
		return;
	}
	if (EXTI->RPR1&(1<<13))
	{
		EXTI->RPR1=1<<13;
		PHY_SX1280_DIO1IRQHandler();
		return;
	}
	return;
}
#elif defined(PN_INR6_HS) || defined(PN_TMR)  || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
void EXTI4_15_IRQHandler(void)
{
	if (EXTI->FPR1&(1<<12))
	{
		EXTI->FPR1=1<<12;
		SPI_SX1280_BusyIRQHandler();
		return;
	}
	if (EXTI->RPR1&(1<<8))
	{
		EXTI->RPR1=1<<8;
		PHY_SX1280_DIO1IRQHandler();
		return;
	}
	return;
}
#elif defined(PN_GMR)
void EXTI0_1_IRQHandler(void)
{
	if (EXTI->PR&(1<<0))
	{
		EXTI->PR=1<<0;
		SPI_SX1280_BusyIRQHandler();
		return;
	}
	return;
}

void EXTI2_3_IRQHandler(void)
{
	if (EXTI->PR&(1<<2))
	{
		EXTI->PR=1<<2;
		PHY_SX1280_DIO1IRQHandler();
		return;
	}
	return;
}
#endif

#if SES_NPT_NB_PORTS!=0
void IB1_GetParamResponse(unsigned char ID,unsigned short ParamType,unsigned long ParamLength,const void *pParamValue)
{
	sSES_CA_IBus1GetParamResponse Response;
	
	Response.ID=ID;
	Response.ParamType=ParamType;
	memcpy(&Response.ParamValue,pParamValue,ParamLength);
	SES_SendResponse(SES_CC_IBUS1_GET_PARAM | SES_CC_RESPONSE | SES_CC_REMOTE | SES_CC_SRC_APP | SES_CC_DST_APP,
		&Response,sizeof(sSES_CA_IBus1GetParamResponse)-4+ParamLength);
	return;
}

void IB1_SetParamResponse(unsigned char ID,unsigned short ParamType)
{
	sSES_CA_IBus1SetParamResponse Response;

	Response.ID=ID;
	Response.ParamType=ParamType;
	SES_SendResponse(SES_CC_IBUS1_SET_PARAM | SES_CC_RESPONSE | SES_CC_REMOTE | SES_CC_SRC_APP | SES_CC_DST_APP,&Response,sizeof(sSES_CA_IBus1SetParamResponse));
	return;
}

void IB1_SetupServoResponse(unsigned char ChannelNb,unsigned char ID,unsigned char OutputNb)
{
	sSES_CA_IBusSetupServoResponse Response;

	Response.ChannelNb=ChannelNb;
	Response.ID=ID;
	Response.OutputNb=OutputNb;
	SES_SendResponse(SES_CC_IBUS_SETUP_SERVO | SES_CC_RESPONSE | SES_CC_REMOTE | SES_CC_SRC_APP | SES_CC_DST_APP,
		&Response,sizeof(sSES_CA_IBusSetupServoResponse));
	return;
}

void IB2_GetParamResponse(const sSES_CA_IBus2GetParamResponse *pResponse,unsigned long ParamLength)
{
	SES_SendResponse(SES_CC_IBUS2_GET_PARAM | SES_CC_RESPONSE | SES_CC_REMOTE | SES_CC_SRC_APP | SES_CC_DST_APP,
		pResponse,sizeof(sSES_CA_IBus2GetParamResponse)-EB2_MAX_PARAM_LENGTH+ParamLength);
	return;
}

void IB2_SetParamResponse(const sSES_CA_IBus2SetParamResponse *pResponse)
{
	SES_SendResponse(SES_CC_IBUS2_SET_PARAM | SES_CC_RESPONSE | SES_CC_REMOTE | SES_CC_SRC_APP | SES_CC_DST_APP,pResponse,sizeof(sSES_CA_IBus2SetParamResponse));
	return;
}
#endif

BOOL NPT_GetInternalSensor(sNPT_Sensor *pSensor)
{
	sLNK_RXConditions RXConditions;
	eIB_DeviceType Type;
	unsigned char ValueLength;

	LNK_GetLastRXConditions(&RXConditions);
	switch (pSensor->Address)
	{
	// Internal voltage
	case 0x80+IB_RISN_INT_VOLTAGE:
		SYS_STORE_USHORT(&pSensor->Value,DIV10(ADC_GetInternalVoltage()))
		Type=IBDT_INT_VOLTAGE;
		ValueLength=2;
		break;
	// Signal strength
	case 0x80+IB_RISN_SIGNAL_STRENGTH:
		pSensor->Value[0]=RXConditions.SignalStrength;
		Type=IBDT_SIGNAL_STRENGTH;
		ValueLength=1;
		break;
	// RSSI
	case 0x80+IB_RISN_RSSI:
		SYS_STORE_USHORT(&pSensor->Value,RXConditions.Phy.SignalLevel)
		Type=IBDT_RSSI;
		ValueLength=2;
		break;
	// Background noise
	case 0x80+IB_RISN_BG_NOISE:
		SYS_STORE_USHORT(&pSensor->Value,RXConditions.Phy.BackgroundNoise)
		Type=IBDT_BG_NOISE;
		ValueLength=2;
		break;
	// SNR
	case 0x80+IB_RISN_SNR:
		if (RXConditions.Phy.BackgroundNoise!=PHY_BACKGROUND_NOISE_UNKNOWN &&
			RXConditions.Phy.SignalLevel!=PHY_SIGNAL_LEVEL_UNKNOWN)
			SYS_STORE_USHORT(&pSensor->Value,RXConditions.Phy.BackgroundNoise-RXConditions.Phy.SignalLevel)
		else
			SYS_STORE_USHORT(&pSensor->Value,LNK_SNR_UNKNOWN)
		Type=IBDT_SNR;
		ValueLength=2;
		break;
		
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B)  || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D) || defined(PN_FBR8)
	// External voltage
	case 0x80+IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS:
		SYS_STORE_USHORT(&pSensor->Value,DIV10(ADC_GetExternalVoltage()))
		Type=IBDT_EXT_VOLTAGE;
		ValueLength=2;
		break;
#endif
	
#if defined(PN_INR4_GYB) || defined(PN_GMR)
	#if defined(PN_INR4_GYB)
		case 0x80+IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS+1:
	#elif defined(PN_GMR)
		case 0x80+IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS:
	#endif
		// 1-axis gyroscope
		// if (Gyroscope unknown) // !!!
		// 	return FALSE;
		// Store real gyroscope value !!!
		SYS_STORE_USHORT(pSensor->Value,Gyro_SVC_Vars.GyroZdata) 
		Type=IBDT_GYROSCOPE_1_AXIS;
		ValueLength=2;
		break;
#endif
		
#if defined(PN_INR6_HS)  
	// Pressure & Temperature
	case 0x80+IB_NB_MANDATORY_RECEIVER_INTERNAL_SENSORS+1:
		*(sIB_AS_Pressure *)pSensor->Value=Bmp280_AS_Pressure;
		Type=IBDT_PRESSURE;
		ValueLength=4;
		break;
#endif
	
	default:
		return FALSE;
	}
	pSensor->Type=Type;
	pSensor->ValueLength=ValueLength;
	return TRUE;
}

static void LNK_FactorySaveConfigCallback(void)
{
	LNK_FactoryConfigChanged=TRUE;
	return;
}

static void SES_BindCallback(BOOL Success)
{
	if (!Success)
		APP_Bind();
	// Factory mode
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_FACTORY ||
		APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_FACTORY_NO_CALIBRATION)
	{
		APP_Binding=FALSE;
		LED_SetPattern(&LED_Factory);
		SES_FactoryApp.TransportFactory.LinkFactory.RFPower=(signed short)APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.Factory.RFPower;
		SES_FactoryApp.TransportFactory.LinkFactory.PAOn=APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.Factory.PAOn;
		SES_FactoryApp.TransportFactory.LinkFactory.NoCalibration=
			(APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_FACTORY)?FALSE:TRUE;
		APP_Config.InternalBatteryMonitorCorrection=0;
		APP_Config.ExternalBatteryMonitorCorrection=0;
		SES_Factory(&SES_FactoryApp);
		return;
	}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// i-Bus 1 PWM adapater mode
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
	// i-Bus 2 PWM adapater mode
		APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
	{
		APP_Binding=FALSE;
		SES_Idle();
		SES_UnpackPWMFrequencies(&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMFrequencies[0],&APP_PWMFrequenciesV1,FALSE);
		SES_UnpackPWMFrequencies(&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMFrequencies[1],&APP_PWMFrequenciesV1,TRUE);
		APP_ActivateReceiverRole=(APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER)?
			LNK_RR_I_BUS_1_PWM_ADAPTER:LNK_RR_I_BUS_2_PWM_ADAPTER;
	}
#endif
	// Receiver
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
	{
		APP_SecondChannelsSetActive=FALSE;
		SES_RunStruct.pChannels=APP_Channels1;
		SES_RunStruct.pChannelsPacked=APP_ChannelsPacked1;
		if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.FeedbackRatio==0 ||
			(APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.Version==1 &&
			 APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFV1.Has2Receivers &&
			 APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFV1.IsAuxiliaryReceiver))
		{
			{
				unsigned char RXFirstPWMRCChannelNb;

				RXFirstPWMRCChannelNb=LNK_BoundConfig1Way.V1.RXFirstPWMRCChannelNb;
				if (RXFirstPWMRCChannelNb>=SES_NB_MAX_CHANNELS)
					RXFirstPWMRCChannelNb=0;
				APP_ConfigRF.RXFirstPWMRCChannelNb=RXFirstPWMRCChannelNb;
			}
			APP_ConfigRF.IsFailsafeInNoOutputMode=LNK_BoundConfig1Way.V1.IsFailsafeInNoOutputMode;
			memcpy(APP_ConfigRF.PWMFrequencies,LNK_BoundConfig1Way.V1.PWMFrequencies,sizeof(APP_ConfigRF.PWMFrequencies));
			SES_UnpackPWMFrequencies(&APP_ConfigRF.PWMFrequencies[0],&APP_PWMFrequenciesV1,FALSE);
			SES_UnpackPWMFrequencies(&APP_ConfigRF.PWMFrequencies[1],&APP_PWMFrequenciesV1,TRUE);
			memcpy(APP_Config.NewPortTypes,LNK_BoundConfig1Way.V1.NewPortTypes,sizeof(APP_Config.NewPortTypes));
			APP_ConfigRF.WSUSARTConfig=APP_WS_DefaultUSARTConfig;
#if defined(PN_INR4_GYB) || defined(PN_GMR)
			APP_ConfigRF.SVC=APP_DefaultSVC;  
            APP_ConfigRF.AdvancedSVC= APP_DefaultAdvancedSVC;
            Gyro_SVC_Vars.CalibrationMidleRestFlg=TRUE;
#endif
			APP_Binding=FALSE;
			APP_SignalStatus=SES_SS_OUT_OF_SYNC;
			LED_SetPattern(&LED_Synchronizing);
			APP_ConfigChanged=TRUE;
			APP_SES_Run(TRUE);
		}
		else
			APP_SES_Run(FALSE);
	}
	else
		APP_Bind();
	return;
}

static unsigned char SES_BuildRealtimePacketCallback(void *pPayload)
{
	if (pPayload)
	{
		if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFV1.Has2Receivers &&
			APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFV1.IsAuxiliaryReceiver)
		{
			if (NPT_BuildRealtimePacketCallback(pPayload,TRUE))
				return SES_CMD_SENSORS_AUX;
		}
		else
		{
			if (NPT_BuildRealtimePacketCallback(pPayload,FALSE))
				return SES_CMD_SENSORS;
		}
	}
	return SES_CMD_NONE;
}
//			unsigned char ResponseLenth, Responsetemp[16];
//			unsigned long Responsedataddr;
static void SES_CommandReceivedCallback(unsigned short Code,const void *pArgument,unsigned long ArgumentLength)
{
	switch (Code & SES_CC_CODE_MASK)
	{
	// Set failsafe (notification only)
	case SES_CC_SET_FAILSAFE:
		if (!APP_Binding)
		{
			APP_ConfigChanged=TRUE;
			NPT_FailsafeChanged();
		}
		return;
	// Set failsafe timeout (notification only)
	case SES_CC_SET_FAILSAFE_TIMEOUT:
		if (!APP_Binding)
			APP_ConfigChanged=TRUE;
		return;
	// Set RF power (notification only)
	case SES_CC_SET_RF_POWER:
		APP_ConfigChanged=TRUE;
		return;
	// Get capabilities
	case SES_CC_GET_CAPABILITIES:
		if (ArgumentLength!=0)
			SES_CommandReceivedCallbackInvalid(Code);
		else
			SES_SendResponse(Code,&SES_Capabilities,sizeof(SES_Capabilities));
		return;
	case SES_CC_SET_RX_FIRST_PWM_RC_CHANNEL_NB:
		if (ArgumentLength!=sizeof(sSES_CA_SetRXFirstPWMRCChannelNb))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			unsigned char RXFirstPWMRCChannelNb;

			RXFirstPWMRCChannelNb=((const sSES_CA_SetRXFirstPWMRCChannelNb *)pArgument)->ChannelNb;
			if (RXFirstPWMRCChannelNb>=SES_NB_MAX_CHANNELS)
				RXFirstPWMRCChannelNb=0;
			if (APP_Binding ||
				RXFirstPWMRCChannelNb!=APP_ConfigRF.RXFirstPWMRCChannelNb)
			{
				APP_ConfigRF.RXFirstPWMRCChannelNb=RXFirstPWMRCChannelNb;
				if (!APP_Binding)
				{
					NPT_SetPWMFrequencies();
					APP_ConfigChanged=TRUE;
				}
			}
			SES_SendResponse(Code,NULL,0);
			if (APP_Binding &&
				APP_BindCommandNb==0)
				APP_BindCommandNb=1;
		}
		return;
	// Set PWM frequency
	case SES_CC_SET_PWM_FREQUENCIES_V1:
		if (ArgumentLength!=sizeof(sSES_CA_SetPWMFrequenciesV1) ||
			((const sSES_CA_SetPWMFrequenciesV1 *)pArgument)->LowHigh>=2)
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			const sSES_CA_SetPWMFrequenciesV1 *pSetPWMFrequenciesV1;
			sSES_PWMFrequenciesHalfV1 *pPWMFrequenciesHalfV1;

			pSetPWMFrequenciesV1=(const sSES_CA_SetPWMFrequenciesV1 *)pArgument;
			pPWMFrequenciesHalfV1=&APP_ConfigRF.PWMFrequencies[pSetPWMFrequenciesV1->LowHigh];
			if (APP_Binding ||
				memcmp(&pSetPWMFrequenciesV1->Frequencies,pPWMFrequenciesHalfV1,sizeof(sSES_PWMFrequenciesHalfV1)))
			{
				*pPWMFrequenciesHalfV1=pSetPWMFrequenciesV1->Frequencies;
				SES_UnpackPWMFrequencies(pPWMFrequenciesHalfV1,&APP_PWMFrequenciesV1,pSetPWMFrequenciesV1->LowHigh);
				if (!APP_Binding)
				{
					NPT_SetPWMFrequencies();
					APP_ConfigChanged=TRUE;
				}
			}
			SES_SendResponse(Code,NULL,0);
			if (APP_Binding)
			{
				if (!pSetPWMFrequenciesV1->LowHigh)
				{
					if (APP_BindCommandNb==1)
						APP_BindCommandNb=2;
				}
				else
				{
					if (APP_BindCommandNb==2)
						APP_BindCommandNb=3;
				}
			}
		}
		return;
	// Set New Port type
	case SES_CC_SET_NEW_PORT_TYPE:
		if (ArgumentLength!=sizeof(sSES_CA_SetNewPortTypes))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (APP_Binding)
			{
				memcpy(APP_Config.NewPortTypes,((const sSES_CA_SetNewPortTypes *)pArgument)->Types,sizeof(APP_Config.NewPortTypes));
				if (APP_BindCommandNb==3)
					APP_BindCommandNb=4;
			}
			else if (memcmp(&((const sSES_CA_SetNewPortTypes *)pArgument)->Types,&APP_Config.NewPortTypes,sizeof(sSES_CA_SetNewPortTypes)))
			{
				if (!APP_SwitchNewPortInProgress)
				{
					memcpy(APP_NewPortTypes,((const sSES_CA_SetNewPortTypes *)pArgument)->Types,sizeof(APP_NewPortTypes));
					NPT_StartDeinit();
					APP_SwitchNewPortInProgress=TRUE;
				}
			}
			SES_SendResponse(Code,NULL,0);
		}
		return;
	// Signal strength RC channel number (notification only)
	case SES_CC_SET_SIGNAL_STRENGTH_RC_CHANNEL_NB:
		if (!APP_Binding)
			APP_ConfigChanged=TRUE;
		NPT_FailsafeChanged();
		return;
	// Set supported sensors
	case SES_CC_SET_SUPPORTED_SENSORS:
		if (ArgumentLength!=sizeof(sSES_CA_SetSupportedSensors))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (memcmp(&APP_ConfigRF.SupportedSensors,pArgument,sizeof(sSES_CA_SetSupportedSensors)))
			{
				memcpy(&APP_ConfigRF.SupportedSensors,pArgument,sizeof(sSES_CA_SetSupportedSensors));
				if (!APP_Binding)
					APP_ConfigChanged=TRUE;
			}
			SES_SendResponse(Code,NULL,0);
			if (APP_Binding &&
				APP_BindCommandNb==4)
				APP_BindCommandNb=5;
		}
		return;
	case SES_CC_SET_IS_FAILSAFE_IN_NO_OUTPUT_MODE:
		if (ArgumentLength!=sizeof(sSES_CA_SetIsFailsafeInNoOutputMode))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (APP_Binding ||
				((const sSES_CA_SetIsFailsafeInNoOutputMode *)pArgument)->IsFailsafeInNoOutputMode!=APP_ConfigRF.IsFailsafeInNoOutputMode)
			{
				APP_ConfigRF.IsFailsafeInNoOutputMode=((const sSES_CA_SetIsFailsafeInNoOutputMode *)pArgument)->IsFailsafeInNoOutputMode;
				if (!APP_Binding)
					APP_ConfigChanged=TRUE;
			}
			SES_SendResponse(Code,NULL,0);
			if (APP_Binding &&
				APP_BindCommandNb==5)
			{
				APP_ConfigRF.WSUSARTConfig=APP_WS_DefaultUSARTConfig;
#if defined(PN_INR4_GYB) || defined(PN_GMR)
				APP_ConfigRF.SVC=APP_DefaultSVC;
                APP_ConfigRF.AdvancedSVC= APP_DefaultAdvancedSVC;
                Gyro_SVC_Vars.CalibrationMidleRestFlg=TRUE;
#endif
				APP_Binding=FALSE;
				APP_SignalStatus=SES_SS_SYNCHRONIZED;
				LED_SetPattern(&LED_Synchronized);
				APP_ConfigChanged=TRUE;
			}
		}
		return;
#if SES_NPT_NB_PORTS!=0
	// Start or stop the setup of an i-Bus output servo
	case SES_CC_IBUS_SETUP_SERVO:
		if (ArgumentLength!=sizeof(sSES_CA_IBusSetupServo))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (!IB1_SetupServo(((const sSES_CA_IBusSetupServo *)pArgument)->ChannelNb))
				SES_CommandReceivedCallbackInvalid(Code);
		}
		return;
#endif
	// Signal status change (notification only)
	case SES_CC_SIGNAL_STATUS_CHANGE:
		if (ArgumentLength!=sizeof(eSES_PA_SignalStatus))
		{
			SES_CommandReceivedCallbackInvalid(Code);
			return;
		}
		if (APP_SignalStatus==((const eSES_PA_SignalStatus *)pArgument)->Status)
			return;
		APP_SignalStatus=((const eSES_PA_SignalStatus *)pArgument)->Status;
		if (APP_Binding)
		{
			if (APP_SignalStatus!=SES_SS_SYNCHRONIZED)
				APP_Bind();
			return;
		}
		switch (APP_SignalStatus)
		{
			case SES_SS_SYNCHRONIZED:
				LED_SetPattern(&LED_Synchronized);
				return;
			case SES_SS_OUT_OF_SYNC:
				LED_SetPattern(&LED_Synchronizing);
				return;
			case SES_SS_FAILSAFE:
				return;
		}
	case SES_CC_TEST:
		if (ArgumentLength!=sizeof(sSES_CA_Test))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			SES_TestStruct=*(const sSES_Test *)pArgument;
			SES_SendResponse(Code,NULL,0);
			APP_TestMs=SYS_SysTickMs;
		}
		return;
	case SES_CC_GET_VERSION:
		if (ArgumentLength!=0)
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sSES_CA_GetVersionResponse Version;
			
			Version.ProductNumber=BOOT_ProductNumber;
			Version.MainboardVersion=BOOT_MainboardVersion;
			Version.RFModuleVersion=BOOT_RFModuleVersion;
			Version.BootloaderVersion=BOOT_BootloaderVersion;
			Version.FirmwareVersion=FIRMWARE_Version;
			Version.RFLibraryVersion=0x0301;//SES_GetVersion();
			SES_SendResponse(Code,&Version,sizeof(sSES_CA_GetVersionResponse));
		}
		return;
#if defined(PN_INR4_GYB) || defined(PN_GMR)
	case SES_CC_SET_SVC:
		if (ArgumentLength!=sizeof(sSES_CA_SVC))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (memcmp(&APP_ConfigRF.SVC,pArgument,sizeof(sSES_CA_SVC)))
			{
				memcpy(&APP_ConfigRF.SVC,pArgument,sizeof(sSES_CA_SVC));
				APP_ConfigChanged=TRUE;
			}
			SES_SendResponse(Code,NULL,0);
		}
		return;		
	case SES_CC_CALIBRATE_SVC:
		if (ArgumentLength!=sizeof(sSES_CA_CalibrateSVC))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sSES_CA_CalibrateSVCResponse Response;
            sSES_CA_CalibrateSVC CalibrateSVC;
            
            memcpy(&CalibrateSVC,pArgument,sizeof(sSES_CA_CalibrateSVC));
            Gyro_SVC_CalibrationBackground(&Gyro_SVC_Vars,&CalibrateSVC,&Response);
//			// Check sSES_CA_CalibrateSVC structure content !!!
//			Response.Status=SES_CSVCS_NO_CALIBRATION; // !!!
			SES_SendResponse(Code,&Response,sizeof(sSES_CA_CalibrateSVC));
		}
		return;	
	case SES_CC_SET_ADVANCED_SVC:
		if (ArgumentLength!=sizeof(sSES_CA_ADVANCED_SVC))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (memcmp(&APP_ConfigRF.AdvancedSVC,pArgument,sizeof(sSES_CA_ADVANCED_SVC)))
			{
				memcpy(&APP_ConfigRF.AdvancedSVC,pArgument,sizeof(sSES_CA_ADVANCED_SVC));
				APP_ConfigChanged=TRUE;
			}
			SES_SendResponse(Code,NULL,0);
		}
		return;	       
#endif
#if defined(PN_INR6_FC)
		case SES_CC_SET_RX_FLIGHT_CONTROLLER_PARAM:
		{		
			unsigned char ResponseLenth, Responsetemp[16];
			unsigned long Responsedataddr;

			FC_TxSetBackground(pArgument,ArgumentLength,&Responsedataddr,&ResponseLenth);
			memset(&Responsetemp,0x00,sizeof(Responsetemp));			
			memcpy(&Responsetemp,&((sSES_CA_FC*)Responsedataddr)->FCList,0x02);			
			memcpy(&Responsetemp[2],((sSES_CA_FC*)Responsedataddr)->FC_CMD_Data,((sSES_CA_FC*)Responsedataddr)->FC_CMD_DataLenth);					
			SES_SendResponse(Code,Responsetemp,ResponseLenth);
		}
		return;						
#endif
	case SES_CC_UPGRADE_FIRMWARE:
		if (ArgumentLength!=0)
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			SES_SendResponse(Code,NULL,0);
			APP_UpdateFirmware=TRUE;
		}
		return;
#if SES_NPT_NB_PORTS!=0
	case SES_CC_IBUS1_SET_PARAM:
		if (ArgumentLength!=sizeof(sSES_CA_IBus1SetParam)-4+1 &&
			ArgumentLength!=sizeof(sSES_CA_IBus1SetParam)-4+2 &&
			ArgumentLength!=sizeof(sSES_CA_IBus1SetParam)-4+4)
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sIB1_SetParam Param;
			
			Param.ID=((const sSES_CA_IBus1SetParam *)pArgument)->ID;
			Param.ParamType=((const sSES_CA_IBus1SetParam *)pArgument)->ParamType;
			Param.ParamLength=(unsigned char)(ArgumentLength-(sizeof(sSES_CA_IBus1SetParam)-4));
			memcpy(&Param.ParamValue,&((const sSES_CA_IBus1SetParam *)pArgument)->ParamValue,Param.ParamLength);
			if (!IB1_SetParam(&Param))
				SES_CommandReceivedCallbackInvalid(Code);
		}
		return;
	case SES_CC_IBUS1_GET_PARAM:
		if (ArgumentLength!=sizeof(sSES_CA_IBus1GetParam))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sIB1_GetParam Param;
			
			Param.ID=((const sSES_CA_IBus1GetParam *)pArgument)->ID;
			Param.ParamType=((const sSES_CA_IBus1GetParam *)pArgument)->ParamType;
			if (!IB1_GetParam(&Param))
				SES_CommandReceivedCallbackInvalid(Code);
		}
		return;
	case SES_CC_IBUS2_SET_PARAM:
		if (ArgumentLength<sizeof(sSES_CA_IBus2SetParam)-EB2_MAX_PARAM_LENGTH+1 ||
			ArgumentLength>sizeof(sSES_CA_IBus2SetParam))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sIB2_SetParam Param;
			
			Param.ID=((const sSES_CA_IBus2SetParam *)pArgument)->ID;
			Param.ParamType=((const sSES_CA_IBus2SetParam *)pArgument)->ParamType;
			Param.ParamLength=(unsigned char)(ArgumentLength-(sizeof(sSES_CA_IBus2SetParam)-EB2_MAX_PARAM_LENGTH));
			memcpy(&Param.ParamValue,&((const sSES_CA_IBus2SetParam *)pArgument)->ParamValue,Param.ParamLength);
			if (!IB2_SetParam(&Param))
				SES_CommandReceivedCallbackInvalid(Code);
		}
		return;
	case SES_CC_IBUS2_GET_PARAM:
		if (ArgumentLength!=sizeof(sSES_CA_IBus2GetParam))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sIB2_GetParam Param;
			
			Param.ID=((const sSES_CA_IBus2GetParam *)pArgument)->ID;
			Param.ParamType=((const sSES_CA_IBus2GetParam *)pArgument)->ParamType;
			if (!IB2_GetParam(&Param))
				SES_CommandReceivedCallbackInvalid(Code);
		}
		return;
#endif
	case SES_CC_CALIBRATE_VOLTAGE_MONITOR_V1:
		if (ArgumentLength!=sizeof(sSES_CA_CalibrateVoltageMonitorV1))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			sSES_CA_CalibrateVoltageMonitorV1Response Response;

			if (((const sSES_CA_CalibrateVoltageMonitorV1 *)pArgument)->ActualInternalVoltage==0)
			{
				APP_Config.InternalBatteryMonitorCorrection=0;
				Response.InternalVoltageCorrection=LNK_FactoryConfig.InternalBatteryMonitorCorrection/2;
			}
			else if (((const sSES_CA_CalibrateVoltageMonitorV1 *)pArgument)->ActualInternalVoltage==0xFFFFFFFF)
			{
				Response.InternalVoltageCorrection=(APP_Config.InternalBatteryMonitorCorrection!=0)?
					APP_Config.InternalBatteryMonitorCorrection:LNK_FactoryConfig.InternalBatteryMonitorCorrection/2;
			}
			else
			{
				unsigned long Correction;
				unsigned long OriginalCorrection;

				OriginalCorrection=APP_Config.InternalBatteryMonitorCorrection;
				APP_Config.InternalBatteryMonitorCorrection=1<<14;
				Correction=(((const sSES_CA_CalibrateVoltageMonitorV1 *)pArgument)->ActualInternalVoltage*(1<<14))/ADC_GetInternalVoltage();
				if (Correction>(1<<15)) // 2.0
				{
					Correction=0xFFFF;
					APP_Config.InternalBatteryMonitorCorrection=(unsigned short)OriginalCorrection;
				}
				else if (Correction<(1<<13)) // 0.5
				{
					Correction=0;
					APP_Config.InternalBatteryMonitorCorrection=(unsigned short)OriginalCorrection;
				}
				else
					APP_Config.InternalBatteryMonitorCorrection=(unsigned short)Correction;
				Response.InternalVoltageCorrection=(unsigned short)Correction;
			}
			if (((const sSES_CA_CalibrateVoltageMonitorV1 *)pArgument)->ActualExternalVoltage==0)
			{
				APP_Config.ExternalBatteryMonitorCorrection=0;
				Response.ExternalVoltageCorrection=LNK_FactoryConfig.ExternalBatteryMonitorCorrection/2;
			}
			else if (((const sSES_CA_CalibrateVoltageMonitorV1 *)pArgument)->ActualExternalVoltage==0xFFFFFFFF)
			{
				Response.ExternalVoltageCorrection=(APP_Config.ExternalBatteryMonitorCorrection!=0)?
					APP_Config.ExternalBatteryMonitorCorrection:LNK_FactoryConfig.ExternalBatteryMonitorCorrection/2;
			}
			else
			{
				unsigned long Correction;
				unsigned long OriginalCorrection;

				OriginalCorrection=APP_Config.ExternalBatteryMonitorCorrection;
				APP_Config.ExternalBatteryMonitorCorrection=1<<14;
				Correction=(((const sSES_CA_CalibrateVoltageMonitorV1 *)pArgument)->ActualExternalVoltage*(1<<14))/ADC_GetExternalVoltage();
				if (Correction>(1<<15)) // 2.0
				{
					Correction=0xFFFF;
					APP_Config.ExternalBatteryMonitorCorrection=(unsigned short)OriginalCorrection;
				}
				else if (Correction<(1<<13)) // 0.5
				{
					Correction=0;
					APP_Config.ExternalBatteryMonitorCorrection=(unsigned short)OriginalCorrection;
				}
				else
					APP_Config.ExternalBatteryMonitorCorrection=(unsigned short)Correction;
				Response.ExternalVoltageCorrection=(unsigned short)Correction;
			}
			APP_ConfigChanged=TRUE;
			SES_SendResponse(Code,&Response,sizeof(sSES_CA_CalibrateVoltageMonitorV1Response));
		}
		return;
	case SES_CC_SET_WS_USART_CONFIG:
		if (ArgumentLength!=sizeof(sSES_CA_SetWSUSARTConfig))
			SES_CommandReceivedCallbackInvalid(Code);
		else
		{
			if (memcmp(&((const sSES_CA_SetWSUSARTConfig *)pArgument)->Config,&APP_ConfigRF.WSUSARTConfig,sizeof(sUSART_Config)))
			{
				APP_ConfigRF.WSUSARTConfig=((const sSES_CA_SetWSUSARTConfig *)pArgument)->Config;
				NPT_SetWSUSARTConfig();
				APP_ConfigChanged=TRUE;
			}
			SES_SendResponse(Code,NULL,0);
		}
		return;
	default:
		if ((Code & SES_CC_DST_SA_MASK)==SES_CC_DST_APP)
			SES_CommandReceivedCallbackNotSupported(Code);
		return;
	}
}

static void SES_CommandReceivedCallbackInvalid(unsigned short Code)
{
	SES_SendResponse((Code & ~(SES_CC_CR_MASK | SES_CC_CODE_MASK)) | SES_CC_RESPONSE | SES_CC_COMMAND_INVALID,&Code,sizeof(Code));
	return;
}

static void SES_CommandReceivedCallbackNotSupported(unsigned short Code)
{
	SES_SendResponse((Code & ~(SES_CC_CR_MASK | SES_CC_CODE_MASK)) | SES_CC_RESPONSE | SES_CC_COMMAND_NOT_SUPPORTED,&Code,sizeof(Code));
	return;
}

static void SES_HeartbeatCallback(void)
{
	if (!APP_ChannelsReceived ||
		APP_SetChannelsCallbackTimeMs==0 ||
		SYS_SysTickMs-APP_SetChannelsCallbackTimeMs<APP_SetChannelsMaxTime)
	{
		return;
	}
	SYS_QueueLowPriorityIRQCallback(SES_SetChannelsCallback,0);
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
static BOOL SES_PreSetChannelsCallback(signed short *pChannels,BOOL IsFailsafe)
#pragma clang diagnostic warning "-Wunused-parameter"
{
    #if defined(PN_INR4_GYB) || defined(PN_GMR)
		if(IsFailsafe)
			return FALSE;
        memcpy(&Gyro_SVC_Vars.SteeringChInput,pChannels,2*sizeof(Gyro_SVC_Vars.SteeringChInput));           
        memcpy(pChannels,&Gyro_SVC_Vars.SteeringChOutput,2*sizeof(Gyro_SVC_Vars.SteeringChOutput));
        return TRUE;   
    #elif defined(PN_INR6_FC) 
		if(IsFailsafe)
			return FALSE;
        memcpy(&FC_Vars.Control.RollChInput,pChannels,4*sizeof(FC_Vars.Control.RollChInput));  
        if(FC_Vars.FC_TXCMD.ModeSwitchChn>4)
        {
            memcpy(&FC_Vars.Control.ModeChInput,pChannels+FC_Vars.FC_TXCMD.ModeSwitchChn-1,sizeof(FC_Vars.Control.ModeChInput));
        }
        
        memcpy(pChannels,&FC_Vars.Control.RollChOutput,4*sizeof(FC_Vars.Control.RollChOutput));
        return TRUE;
	#else		
        return FALSE;   
    #endif
}

#pragma clang diagnostic ignored "-Wunused-parameter"
static void SES_RealtimePacketReceivedCallback(unsigned char Command,const void *pPayload)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	if (APP_Binding)
		return;
	if (Command==SES_CMD_FAILSAFE)
	{
		APP_ConfigChanged=TRUE;
		NPT_FailsafeChanged();
		return;
	}
	// Only auxiliary or one-way receivers process these real-time packets
	if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.FeedbackRatio!=0 &&
		(!APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFV1.Has2Receivers ||
		 !APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFV1.IsAuxiliaryReceiver))
	{
		return;
	}
	switch (Command)
	{
	// Common configuration
	case SES_CMD_V1_CONFIG_1:
	{
		const sSES_RTPA_ReceiverV1Config *pReceiverV1Config;
		
		pReceiverV1Config=(const sSES_RTPA_ReceiverV1Config *)pPayload;
		if (memcmp(APP_Config.NewPortTypes,pReceiverV1Config->NewPortTypes,sizeof(APP_Config.NewPortTypes)))
		{
			if (!APP_SwitchNewPortInProgress)
			{
				memcpy(APP_NewPortTypes,pReceiverV1Config->NewPortTypes,sizeof(APP_NewPortTypes));
				NPT_StartDeinit();
				APP_SwitchNewPortInProgress=TRUE;
			}
		}
		if (APP_ConfigRF.RXFirstPWMRCChannelNb!=pReceiverV1Config->RXFirstPWMRCChannelNb)
		{
			APP_ConfigRF.RXFirstPWMRCChannelNb=pReceiverV1Config->RXFirstPWMRCChannelNb;
			NPT_SetPWMFrequencies();
			APP_ConfigChanged=TRUE;
		}
		if (APP_ConfigRF.IsFailsafeInNoOutputMode!=pReceiverV1Config->IsFailsafeInNoOutputMode)
		{
			APP_ConfigRF.IsFailsafeInNoOutputMode=pReceiverV1Config->IsFailsafeInNoOutputMode;
			APP_ConfigChanged=TRUE;
		}
		return;
	}
#if defined(PN_INR4_GYB) || defined(PN_GMR)
	// SVC
	case SES_CMD_SVC:
	{
		const sSES_CA_SVC *pSVC;
		
		pSVC=(const sSES_CA_SVC *)pPayload;
		if (memcmp(&APP_ConfigRF.SVC,pSVC,sizeof(sSES_CA_SVC)))
		{
			memcpy(&APP_ConfigRF.SVC,pSVC,sizeof(sSES_CA_SVC));
			APP_ConfigChanged=TRUE;
		}
		return;
	}
#endif
	// PWM frequency
	case SES_CMD_PWM_FREQUENCY:
	{
		BOOL ConfigChanged;
		const sSES_RTPA_PWMFrequencies *pPWMFrequencies;

		ConfigChanged=FALSE;
		pPWMFrequencies=(const sSES_RTPA_PWMFrequencies *)pPayload;
		{
			unsigned long ChannelNb;

			for (ChannelNb=0;ChannelNb<SES_NB_REALTIME_PWM_FREQUENCY_CHANNELS;ChannelNb++)
			{
				unsigned long BitNb;
				unsigned long Frequency;

				BitNb=ChannelNb*SES_PWM_FREQUENCY_V1_NB_BITS;
				Frequency=(SYS_LoadUShort(pPWMFrequencies->PWMFrequencies+(BitNb/8))>>(BitNb%8)) & 0x1FF;
				if (APP_PWMFrequenciesV1.PWMFrequencies[pPWMFrequencies->FirstChannelNb+ChannelNb]!=Frequency)
				{
					APP_PWMFrequenciesV1.PWMFrequencies[pPWMFrequencies->FirstChannelNb+ChannelNb]=(unsigned short)Frequency;
					ConfigChanged=TRUE;
				}
			}
		}
		if (((APP_PWMFrequenciesV1.Synchronized>>pPWMFrequencies->FirstChannelNb)&((1<<SES_NB_REALTIME_PWM_FREQUENCY_CHANNELS)-1))!=
			pPWMFrequencies->Synchronized)
		{
			APP_PWMFrequenciesV1.Synchronized=
				(APP_PWMFrequenciesV1.Synchronized&~(((1UL<<SES_NB_REALTIME_PWM_FREQUENCY_CHANNELS)-1)<<pPWMFrequencies->FirstChannelNb)) |
				((unsigned long)pPWMFrequencies->Synchronized<<pPWMFrequencies->FirstChannelNb);
			ConfigChanged=TRUE;
		}
		if (ConfigChanged)
		{
			NPT_SetPWMFrequencies();
			APP_ConfigChanged=TRUE;
		}
		return;
	}
	// Short command
	case SES_CMD_SHORT_COMMAND:
	{
		const sSES_RTPA_ShortCommand *pShortCommand;

		pShortCommand=(const sSES_RTPA_ShortCommand*)pPayload;
		APP_SESCommand.Code=(pShortCommand->Code & ~SES_CC_LR_MASK) | SES_CC_LOCAL;
		APP_SESCommand.ArgumentLength=pShortCommand->ArgumentLength;
		APP_SESCommand.pCallback=SES_SEND_COMMAND_DUMMY_CALLBACK;
		APP_SESCommand.Timeout=0;
		memcpy(APP_SESCommand.Argument,pShortCommand->Argument,sizeof(pShortCommand->Argument));
		SES_SendCommand(&APP_SESCommand);
		return;
	}
	default:
		return;
	}
}

#pragma clang diagnostic ignored "-Wunused-parameter"
static void SES_SetChannelsCallback(unsigned long CallbackValue)
#pragma clang diagnostic warning "-Wunused-parameter"
{
	if (APP_Binding)
		return;
	APP_SetChannelsCallbackTimeMs=SYS_SysTickMs;
	{
		signed short *pChannelsOld;
		signed short *pChannelsNew;
		unsigned char *pChannelsPackedOld;
		unsigned char *pChannelsPackedNew;

		if (!APP_SecondChannelsSetActive)
		{
			pChannelsOld=APP_Channels1;
			pChannelsNew=APP_Channels2;
			pChannelsPackedOld=APP_ChannelsPacked1;
			pChannelsPackedNew=APP_ChannelsPacked2;
		}
		else
		{
			pChannelsOld=APP_Channels2;
			pChannelsNew=APP_Channels1;
			pChannelsPackedOld=APP_ChannelsPacked2;
			pChannelsPackedNew=APP_ChannelsPacked1;
		}
		memcpy(pChannelsOld,pChannelsNew,SES_NB_MAX_CHANNELS*2);
		memcpy(pChannelsPackedOld,pChannelsPackedNew,SES_MAX_CHANNELS_LENGTH);
		APP_NPT_InitStructRF.pChannels=pChannelsNew;
		APP_NPT_InitStructRF.pChannelsPacked=pChannelsPackedNew;
		SES_RunStruct.pChannels=pChannelsOld;
		SES_RunStruct.pChannelsPacked=pChannelsPackedOld;
		APP_SecondChannelsSetActive^=TRUE;
	}
	APP_ChannelsReceived=TRUE;
	NPT_SetChannels();
	return;
}

static BOOL SES_WSDataReceivedCallback(const void *pData,unsigned long DataLength)
{
	if (APP_Binding)
		return FALSE;
	return NPT_WSDataReceivedCallback(pData,DataLength);
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void PHY_SetAntenna(unsigned char AntennaNb)
#pragma clang diagnostic warning "-Wunused-parameter"
{
#if defined(PN_FTR8B) || defined(PN_FTR12B) || defined(PN_FBR12)
	if (AntennaNb==0)
	{
		// Set PC13=0 (Antenna 1 switch, active low)
		GPIOC->BSRR=1UL<<(16+13);
		// Set PC15=1 (Antenna 2 switch, active low)
		GPIOC->BSRR=1<<15;
	}
	else
	{
		// Set PC15=0 (Antenna 2 switch, active low)
		GPIOC->BSRR=1UL<<(16+15);
		// Set PC13=1 (Antenna 1 switch, active low)
		GPIOC->BSRR=1<<13;
	}
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)
	if (AntennaNb==0)
	{
		// Set PB3=0 (Antenna 1 switch, active low)
		GPIOB->BSRR=1UL<<(16+3);
		// Set PB4=1 (Antenna 2 switch, active low)
		GPIOB->BSRR=1<<4;
	}
	else
	{
		// Set PB4=0 (Antenna 2 switch, active low)
		GPIOB->BSRR=1UL<<(16+4);
		// Set PB3=1 (Antenna 1 switch, active low)
		GPIOB->BSRR=1<<3;
	}
#elif defined(PN_FGR4D)
	if (AntennaNb==0)
	{
		// Set PB4=0 (Antenna 2 switch, active low)
		GPIOB->BSRR=1UL<<(16+4);
		// Set PB3=1 (Antenna 1 switch, active low)
		GPIOB->BSRR=1<<3;
	}
	else
	{
		// Set PB3=0 (Antenna 1 switch, active low)
		GPIOB->BSRR=1UL<<(16+3);
		// Set PB4=1 (Antenna 2 switch, active low)
		GPIOB->BSRR=1<<4;
	}
#elif defined(PN_TR8B)
	if (AntennaNb==0)
	{
		// Set PA15=0 (Antenna 1 switch, active low)
		GPIOA->BSRR=1UL<<(16+15);
		// Set PB2=1 (Antenna 2 switch, active low)
		GPIOB->BSRR=1<<2;
	}
	else
	{
		// Set PB2=0 (Antenna 2 switch, active low)
		GPIOB->BSRR=1UL<<(16+2);
		// Set PA15=1 (Antenna 1 switch, active low)
		GPIOA->BSRR=1<<15;
	}
#elif defined(PN_FGR4B) || defined(PN_FGR8B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR) || defined(PN_FBR8) || defined(PN_FBR4)
#endif
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void PHY_SetAttenuator(BOOL AttenuatorOn)
#pragma clang diagnostic warning "-Wunused-parameter"
{
#if defined(FS_NONE)
	if (AttenuatorOn)
	{
		// Set PC6=0 (Attenuator high)
		GPIOC->BSRR=1UL<<(16+6);
		// Set PC7=1 (Attenuator low)
		GPIOC->BSRR=1<<7;
	}
	else
	{
		// Set PC7=0 (Attenuator low)
		GPIOC->BSRR=1<<(16+7);
		// Set PC6=1 (Attenuator high)
		GPIOC->BSRR=1UL<<6;
	}
#elif defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
		|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
#endif
	return;
}

#pragma clang diagnostic ignored "-Wunused-parameter"
void PHY_SetPAMode(ePHY_PAMode PAMode)
#pragma clang diagnostic warning "-Wunused-parameter"
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	switch (PAMode)
	{
	case PHY_PAM_IDLE:
		// Set PC14=0 (PA TX switch, active high)
		GPIOC->BSRR=1<<(16+14);
		// Set PA10=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+10);
		break;
	case PHY_PAM_TX:
		// Set PA10=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+10);
		// Set PC14=1 (PA TX switch, active high)
		GPIOC->BSRR=1<<14;
		break;
	case PHY_PAM_RX:
		// Set PC14=0 (PA TX switch, active high)
		GPIOC->BSRR=1<<(16+14);
		// Set PA10=1 (PA RX switch, active high)
		GPIOA->BSRR=1<<10;
		break;
	}
#elif defined(PN_FGR4D)
	switch (PAMode)
	{
	case PHY_PAM_IDLE:
		// Set PB5=0 (PA TX switch, active high)
		GPIOB->BSRR=1<<(16+5);
		// Set PA11=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+11);
		break;
	case PHY_PAM_TX:
		// Set PA11=0 (PA RX switch, active high)
		GPIOA->BSRR=1<<(16+11);
		// Set PB5=1 (PA TX switch, active high)
		GPIOB->BSRR=1<<5;
		break;
	case PHY_PAM_RX:
		// Set PB5=0 (PA TX switch, active high)
		GPIOB->BSRR=1<<(16+5);
		// Set PA11=1 (PA RX switch, active high)
		GPIOA->BSRR=1<<11;
		break;
	}
#elif defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) 
#endif
	return;
}

#if defined(PHY_SX1280)
BOOL PHY_SX1280_IsBusy(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4)
	return (GPIOB->IDR&(1<<14))>>14;
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	return (GPIOA->IDR&(1<<12))>>12;
#elif defined(PN_GMR)
	return (GPIOB->IDR&(1<<0))>>0;
#endif
}

void PHY_SX1280_SetResetPin(BOOL Level)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	GPIOA->BSRR=(Level)?1<<9:1<<(16+9);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FGR4D)
	GPIOB->BSRR=(Level)?1<<9:1<<(16+9);
#elif defined(PN_GMR)
	GPIOA->BSRR=(Level)?1<<0:1<<(16+0);
#endif
	return;
}
#endif
#if defined(PN_TR8B)
void APP_RFModeSwitch(void)
{
	switch (APP_ModeStatus)
	{
		case APP_MODE_NONE:
		{
			break;
		}
		case APP_MODE_RUN:
		{
			// Receiver mode
			if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
			{
				APP_Binding=FALSE;
				LED_SetPattern(&LED_Synchronizing);
				SES_Init(&SES_InitStruct);
				APP_SES_Run(TRUE);
			}
			// PWM adapter mode
			else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
				APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
			{

			}
			// Configuration error
			else
				while (1);
			APP_SysTickMs=SYS_SysTickMs;
			APP_ModeStatus=APP_MODE_WAITSYNCHRONIZED;
			break;
		}
		case APP_MODE_WAITSYNCHRONIZED:
		{
			if(APP_SignalStatus==SES_SS_SYNCHRONIZED)
			{
				APP_ModeStatus=APP_MODE_NONE;
			}
			else if(APP_SysTickMs+APP_MODE_WAITSYNCHRONIZED_TIME<SYS_SysTickMs)
			{
				APP_ModeStatus=APP_MODE_BIND;
			}
			break;
		}
		case APP_MODE_BIND:
		{
			if(APP_SignalStatus != SES_SS_SYNCHRONIZED)
			{
				APP_Bind();
			}
			APP_SysTickMs=SYS_SysTickMs;
			APP_ModeStatus=APP_MODE_WAITBIND;			
			break;
		}
		case APP_MODE_WAITBIND:
		{
			if(APP_SignalStatus == SES_SS_SYNCHRONIZED)
			{
				APP_ModeStatus=APP_MODE_NONE;
			}
			else if(APP_SysTickMs+APP_MODE_WAITBIND_TIME<SYS_SysTickMs)
			{
				memcpy(&APP_Config,&APP_OldConfig,sizeof(sAPP_Config));
				// Receiver mode
				if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
				{
					APP_Binding=FALSE;
					LED_SetPattern(&LED_Synchronizing);
					SES_Init(&SES_InitStruct);
					APP_SES_Run(TRUE);
				}
				// PWM adapter mode
				else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
					APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
				{

				}
				// Configuration error
				else
					while (1);
				APP_ModeStatus=APP_MODE_NONE;
			}		

			break;
		}
//		default:
//			
//			break;
	}	
	return;
}
#endif
static void APP_Background(void)
{
#if defined(PN_TR8B)
	APP_RFModeSwitch();	
#endif
	if (APP_SwitchNewPortInProgress &&
		!NPT_DeinitializationInProgress)
	{
		memcpy(APP_Config.NewPortTypes,APP_NewPortTypes,sizeof(APP_Config.NewPortTypes));
		APP_ConfigChanged=TRUE;
		NPT_Init(&APP_NPT_InitStructRF);
		APP_SwitchNewPortInProgress=FALSE;
	}
	if (APP_ConfigChanged)
	{
		APP_ConfigChanged=FALSE;
		APP_SaveConfig();
	}
	if (LNK_FactoryConfigChanged)
	{
		LNK_FactoryConfigChanged=FALSE;
		APP_SaveFactoryConfig();
	}
	if (APP_TestMs!=0 &&
		SES_IsWSIdle())
	{
		APP_TestMs=0;
		LED_SetPattern(&LED_Test);
		SES_Test(&SES_TestStruct);
	}
	if (APP_UpdateFirmware &&
		SES_IsWSIdle())
	{
		SYS_ResetToBootloader();
	}
	// Manages starting binding before having received the channels from a transmitter
#if !defined(PN_FBR12) && !defined(PN_TR8B) && !defined(PN_FBR8) && !defined(PN_FBR4)
	if (!APP_ChannelsReceived && !APP_Binding)
	{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB)
		if (GPIOA->IDR&(1<<15))
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
		if (GPIOA->IDR&(1<<8))
#elif defined(PN_GMR)
		if (GPIOA->IDR&(1<<12))
#endif
		{
			APP_BindSwitchPressedMs=0;
		}
		else
		{
			if (APP_BindSwitchPressedMs==0)
				APP_BindSwitchPressedMs=SYS_SysTickMs;
			else if (SYS_SysTickMs-APP_BindSwitchPressedMs>=APP_BIND_SWITCH_TIME)
			{
				RAM_BindSignature1=APP_BIND_SIGNATURE_1;
				RAM_BindSignature2=APP_BIND_SIGNATURE_2;
				LED_SetPattern(&LED_WaitBind);
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB)
				while (!(GPIOA->IDR&(1<<15)));
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
				while (!(GPIOA->IDR&(1<<8)));
#elif defined(PN_GMR)
				while (!(GPIOA->IDR&(1<<12)));
#endif
				NVIC_SystemReset();
			}
		}
	}
#endif
	
#if defined(PN_INR6_HS)  
	Bmp280_Background();
#endif

#if defined(PN_INR4_GYB) || defined(PN_GMR)
	Gyro_SVC_Background();
#endif
	
#if  defined(PN_INR6_FC)
	FC_Background();
#endif	
	
	return;
}

static void APP_Bind(void)
{
	APP_Binding=TRUE;
	APP_BindCommandNb=0;
	APP_SignalStatus=SES_SS_OUT_OF_SYNC;
	LED_SetPattern(&LED_Bind);
	SES_Bind(&SES_BindStruct);
	return;
}

static BOOL APP_LoadConfig(void)
{
	APP_ConfigChanged=FALSE;
	if (!EEPROM_Read(0,&APP_Config,sizeof(sAPP_Config)) ||
		APP_Config.Length!=sizeof(sAPP_Config) ||
		APP_Config.FirmwareVersion!=FIRMWARE_Version ||
		APP_Config.CRC32!=SYS_CalcCRC32Table(&APP_Config,sizeof(sAPP_Config)-4))
	{
		memset(&APP_Config,0x00,sizeof(sAPP_Config));
		APP_Config.InternalBatteryMonitorCorrection=0;
		APP_Config.ExternalBatteryMonitorCorrection=0;
		return FALSE;
	}
	// Receiver
	if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
	{
		SES_UnpackPWMFrequencies(&APP_ConfigRF.PWMFrequencies[0],&APP_PWMFrequenciesV1,FALSE);
		SES_UnpackPWMFrequencies(&APP_ConfigRF.PWMFrequencies[1],&APP_PWMFrequenciesV1,TRUE);
	}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// i-Bus 1 or i-Bus 2 PWM adapter
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
		APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
	{
		SES_UnpackPWMFrequencies(&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMFrequencies[0],&APP_PWMFrequenciesV1,FALSE);
		SES_UnpackPWMFrequencies(&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMFrequencies[1],&APP_PWMFrequenciesV1,TRUE);
	}
#endif
	else
		return FALSE;
#if defined(PN_TR8B)
	memcpy(&APP_OldConfig,&APP_Config,sizeof(sAPP_Config));
#endif
	return TRUE;
}

static void APP_LoadFactoryConfig(void)
{
	if (!EEPROM_Read(APP_EEPPROM_SIZE-sizeof(sLNK_FactoryConfig),&LNK_FactoryConfig,sizeof(sLNK_FactoryConfig)) ||
		LNK_FactoryConfig.Length!=sizeof(sLNK_FactoryConfig) ||
		LNK_FactoryConfig.CRC32!=SYS_CalcCRC32Table(&LNK_FactoryConfig,sizeof(sLNK_FactoryConfig)-4))
	{
		LNK_FactoryConfig.InternalBatteryMonitorCorrection=1<<15;
		LNK_FactoryConfig.ExternalBatteryMonitorCorrection=1<<15;
		LNK_FactoryConfig.PHY.TXPowerCorrection=0;
		LNK_FactoryConfig.PHY.RSSICorrection=0;
		LNK_FactoryConfig.PHY.FrequencyOffsetCorrection=0;
	}
	return;
}

void APP_SaveConfig(void)
{
	// Receiver
	if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
	{
		SES_PackPWMFrequencies(&APP_PWMFrequenciesV1,&APP_ConfigRF.PWMFrequencies[0],FALSE);
		SES_PackPWMFrequencies(&APP_PWMFrequenciesV1,&APP_ConfigRF.PWMFrequencies[1],TRUE);
	}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_INR6_FC)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	// i-Bus 1 or i-Bus 2 PWM adapter
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
		APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
	{
		SES_PackPWMFrequencies(&APP_PWMFrequenciesV1,&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMFrequencies[0],FALSE);
		SES_PackPWMFrequencies(&APP_PWMFrequenciesV1,&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.PWMAdapter.PWMFrequencies[1],TRUE);
	}
#endif
	APP_Config.Length=sizeof(sAPP_Config);
	APP_Config.FirmwareVersion=FIRMWARE_Version;
	APP_Config.CRC32=SYS_CalcCRC32Table(&APP_Config,sizeof(sAPP_Config)-4);
	EEPROM_Write(0,&APP_Config,sizeof(sAPP_Config));
	return;
}

static void APP_SaveFactoryConfig(void)
{
	LNK_FactoryConfig.Length=sizeof(sLNK_FactoryConfig);
	LNK_FactoryConfig.CRC32=SYS_CalcCRC32Table(&LNK_FactoryConfig,sizeof(sLNK_FactoryConfig)-4);
	EEPROM_Write(APP_EEPPROM_SIZE-sizeof(sLNK_FactoryConfig),&LNK_FactoryConfig,sizeof(sLNK_FactoryConfig));
	return;
}

static void APP_SES_Run(BOOL CallSES_Run)
{
	unsigned long CycleTime;
	unsigned long NbRFSmallCycles;

	if (CallSES_Run)
		SES_Run(&SES_RunStruct);
	CycleTime=SES_GetRFCycleTime();
	if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.FeedbackRatio!=0)
		NbRFSmallCycles=APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.FeedbackRatio;
	else
		NbRFSmallCycles=1;
	APP_SetChannelsMaxTime=(CycleTime*APP_SET_CHANNELS_MAX_RF_CYCLES)/(NbRFSmallCycles*1000);
	if (APP_SetChannelsMaxTime>APP_SET_CHANNELS_MAX_TIME)
		APP_SetChannelsMaxTime=APP_SET_CHANNELS_MAX_TIME;
	return;
}

static void APP_StartNewPort(void)	
{
	// Receiver mode
	if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
	{
		APP_NPT_InitStructRF.RXPayloadLength=APP_Config.BoundConfig.TransportBoundConfig.RXPayloadLength;
		NPT_Init(&APP_NPT_InitStructRF);
	}
#if !defined(PN_TR8B)
	// PWM adapter mode
	else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
		APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
	{
		NPT_Init(&APP_NPT_InitStructPWM);
	}
#endif
	return;
}

int main(void)
{
	SYS_Init();
#if !defined(DEBUG)	
	EnbleReadProtection();	
	Encryption_Check();
#endif


#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set PA9 as output (SX1280 RESET)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (1<<(9*2));
	// Set PB14 as input (SX1280 BUSY)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(14*2))) | (0<<(14*2));
	// Set PB13 as input (SX1280 DIO1)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(13*2))) | (0<<(13*2));
	// Set an interrupt on PB13 rising (DIO1)
	// Enable external interrupt 13 on port B
	EXTI->EXTICR[13/4]=(EXTI->EXTICR[13/4]&0xFFFF00FF) | 0x00000100;
	// Unmask interrupt request from line 13
	EXTI->IMR1|=1<<13;
	// Enable rising trigger from input line 13
	EXTI->RTSR1|=1<<13;
	// Disable falling trigger from input line 13
	EXTI->FTSR1&=~(1UL<<13);
	// Set an interrupt on PB14 falling (Busy)
	// Enable external interrupt 14 on port B
	EXTI->EXTICR[14/4]=(EXTI->EXTICR[14/4]&0xFF00FFFF) | 0x00010000;
	// Unmask interrupt request from line 14
	EXTI->IMR1|=1<<14;
	// Disable rising trigger from input line 14
	EXTI->RTSR1&=~(1UL<<14);
	// Enable falling trigger from input line 14
	EXTI->FTSR1|=1<<14;
	// Set the EXTI2_3 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI4_15_IRQn,IRQ_PRI_HIGH);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	// Set PB9 as output (SX1280 RESET)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(9*2))) | (1<<(9*2));
	// Set PA12 as input (SX1280 BUSY)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(12*2))) | (0<<(12*2));
	// Set PB8 as input (SX1280 DIO1)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(8*2))) | (0<<(8*2));
	// Set an interrupt on PB8 rising (DIO1)
	// Enable external interrupt 8 on port B
	EXTI->EXTICR[8/4]=(EXTI->EXTICR[8/4]&0xFFFFFF00) | 0x00000001;
	// Unmask interrupt request from line 8
	EXTI->IMR1|=1<<8;
	// Enable rising trigger from input line 8
	EXTI->RTSR1|=1<<8;
	// Disable falling trigger from input line 8
	EXTI->FTSR1&=~(1UL<<8);
	// Set an interrupt on PA12 falling (Busy)
	// Enable external interrupt 12 on port A
	EXTI->EXTICR[12/4]=(EXTI->EXTICR[12/4]&0xFFFFFF00) | 0x00000000;
	// Unmask interrupt request from line 12
	EXTI->IMR1|=1<<12;
	// Disable rising trigger from input line 12
	EXTI->RTSR1&=~(1UL<<12);
	// Enable falling trigger from input line 12
	EXTI->FTSR1|=1<<12;
	// Set the EXTI2_3 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI4_15_IRQn,IRQ_PRI_HIGH);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
#elif defined(PN_GMR)	
	// Set PA0 as output (SX1280 RESET)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (1<<(0*2));

	// Set PB0 as input (SX1280 BUSY)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (0<<(0*2));
	// Enable external interrupt 0 on port B (DIO1)
	SYSCFG->EXTICR[0/4]=(SYSCFG->EXTICR[0/4]&0xFFF0) | 0x0001;
	// Set an interrupt on PB0 falling (Busy)
	// Unmask interrupt request from line 0
	EXTI->IMR|=1<<0;
	// Enable rising trigger from input line 0
	EXTI->RTSR&=~(1UL<<0);
	// Disable falling trigger from input line 0
	EXTI->FTSR|=(1UL<<0);
	// Enable external interrupt 0 on port B (Busy)	
	// Set the EXTI0_1 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI0_1_IRQn,IRQ_PRI_HIGH);
	NVIC->ISER[0]=1UL<<EXTI0_1_IRQn;
	
	// Set PB2 as input (SX1280 DIO1)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (0<<(2*2));
	SYSCFG->EXTICR[2/4]=(SYSCFG->EXTICR[2/4]&0xF0FF) | 0x0100;
	// Set an interrupt on PB2 rising (DIO1)
	// Unmask interrupt request from line 2
	EXTI->IMR|=1<<2;
	// Disable rising trigger from input line 2
	EXTI->RTSR|=(1UL<<2);
	// Enable falling trigger from input line 2
	EXTI->FTSR&=~(1UL<<2);	
	// Set the EXTI2_3 IRQ to high priority and enable it
	NVIC_SetPriority(EXTI2_3_IRQn,IRQ_PRI_HIGH);
	NVIC->ISER[0]=1UL<<EXTI2_3_IRQn;
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) 
	// Set PA15 as input (Bind port)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (0<<(15*2));
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	// Set PA8 as input (Bind port)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(8*2))) | (0<<(8*2));
#elif defined(PN_GMR)
	// Set PA12 as input (Bind port)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(12*2))) | (0<<(12*2));
#endif

#if defined(PN_FTR8B) || defined(PN_FTR12B) || defined(PN_FBR12) 
	// Set PC13 as output (Antenna 1 switch, active low)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(13*2))) | (1<<(13*2));
	// Set PC15 as output (Antenna 2 switch, active low)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(15*2))) | (1<<(15*2));
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_FGR4D)
	// Set PB3 as output (Antenna 1 switch, active low)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(3*2))) | (1<<(3*2));
	// Set PB4 as output (Antenna 2 switch, active low)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(4*2))) | (1<<(4*2));
#elif defined(PN_TR8B)
	// Set PB2 as output (Antenna 1 switch, active low)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (1<<(2*2));
	// Set P15 as output (Antenna 2 switch, active low)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (1<<(15*2));
#elif defined(PN_FGR4B) || defined(PN_FGR8B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_GMR) || defined(PN_FBR8) || defined(PN_FBR4)
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
	// Set PC14 as output (PA TX switch, active high)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(14*2))) | (1<<(14*2));
	// Set PA10 as output (PA RX switch, active high)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (1<<(10*2));
#elif defined(PN_FGR4D)
	// Set PB5 as output (PA TX switch, active high)
	GPIOB->MODER=(GPIOB->MODER&~(3UL<<(5*2))) | (1<<(5*2));
	// Set PA11 as output (PA RX switch, active high)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
#elif defined(PN_INR6_HS) || defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) 
#endif

#if defined(FS_NONE)
	// Set PC7 as output (Attenuator low)
	GPIOC->MODER=(GPIOC->MODER&~(3<<(7*2))) | (1<<(7*2));
	// Set PC6 as output (Attenuator high)
	GPIOC->MODER=(GPIOC->MODER&~(3UL<<(6*2))) | (1<<(6*2));
#elif defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
	|| defined(PN_GMR) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
#endif

	LED_Init();
#if defined(PHY_SX1280)
	SPI_Init();
#endif
	I2C_Init();
	EEPROM_Init();
	ADC_Init();
	NPTTIM_Init();
#if defined(PN_INR6_HS) 
	Bmp280_Init();
#endif
#if defined(PN_INR4_GYB) || defined(PN_GMR)
	Gyro_Init();
#endif
#if defined(PN_INR4_GYB)
	Gyro_SVC_Vars.SupportAdvancedSVC = SES_Capabilities.HasAdvancedSVC;
#endif
#if defined(PN_INR6_FC)
	FC_Init();
#endif
	APP_Binding=TRUE;
	APP_ChannelsReceived=FALSE;
	APP_SetChannelsCallbackTimeMs=0;
	APP_UpdateFirmware=FALSE;
	APP_TestMs=0;
	APP_ConfigChanged=FALSE;
	LNK_FactoryConfigChanged=FALSE;
	APP_BindSwitchPressedMs=0;
	APP_ActivateReceiverRole=LNK_RR_RF_RECEIVER;
	memset(APP_Channels1,0,SES_NB_MAX_CHANNELS*2);
	memset(APP_Channels2,0,SES_NB_MAX_CHANNELS*2);
#if !defined(DEBUG)
	SYS_StartWatchdog();
#endif
	APP_LoadFactoryConfig();
	APP_LoadConfig();
	SES_InitStruct.TransportInit.LinkInit.LocalID=CONFIG_RXID;          //123
	SES_InitStruct.TransportInit.LinkInit.CompanyCode=CONFIG_CompanyCode;  //1
	// Binding or invalid configuration?
#if defined(PN_FBR12) || defined(PN_FBR8) || defined(PN_FBR4)
	if ((RAM_IsBinding &&
		RAM_BindSignature1!=APP_BIND_SIGNATURE_1 &&
		 RAM_BindSignature2!=APP_BIND_SIGNATURE_2) ||
		!APP_LoadConfig())
 #else
  	if (RAM_IsBinding ||
		(RAM_BindSignature1==APP_BIND_SIGNATURE_1 &&
		 RAM_BindSignature2==APP_BIND_SIGNATURE_2) ||
		!APP_LoadConfig())  
#endif    
	{
#if defined(PN_GMR) ||  defined(PN_INR4_GYB)
        if (RAM_IsBinding)
        {
            APP_LoadConfig();
        }       
#endif  
		RAM_BindSignature1=0;
		RAM_BindSignature2=0;
		APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole=LNK_RR_RF_RECEIVER;
		SES_Init(&SES_InitStruct);
#if defined(PN_TR8B)
		APP_ModeStatus=APP_MODE_NONE;
		APP_Bind();
#else
		APP_Bind();
#endif
		while (APP_Binding)
		{
#if !defined(DEBUG)
			SYS_ResetWatchdog();
#endif
		}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
		|| defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		if (APP_ActivateReceiverRole!=LNK_RR_RF_RECEIVER)
			PWMA_Activate(APP_ActivateReceiverRole);
#endif
		APP_StartNewPort();
	}
	else
	{
		APP_Binding=FALSE;
		APP_SignalStatus=SES_SS_OUT_OF_SYNC;
		APP_SecondChannelsSetActive=FALSE;
		APP_StartNewPort();
#if defined(PN_TR8B)
		APP_ModeStatus=APP_MODE_RUN;
#else
		// Receiver mode
		if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_RF_RECEIVER)
		{
			LED_SetPattern(&LED_Synchronizing);
			SES_Init(&SES_InitStruct);
			APP_SES_Run(TRUE);
		}
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS)\
		|| defined(PN_INR6_FC)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		// PWM adapter mode
		else if (APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_1_PWM_ADAPTER ||
			APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.ReceiverRole==LNK_RR_I_BUS_2_PWM_ADAPTER)
		{
			PWMA_Init();
		}
#endif
		// Configuration error
		else
			while (1);
#endif
	}
	while (1)
	{
		APP_Background();
#if !defined(DEBUG)
		SYS_ResetWatchdog();
#endif
	}
}
