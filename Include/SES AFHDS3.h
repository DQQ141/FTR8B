#if !defined(SES_AFHDS3_H)
#define SES_AFHDS3_H

#include "System definitions.h"

#include "External bus.h"
#include "SES AFHDS3 external.h"
#include "i-Bus.h"
#include "i-Bus 2.h"
#include "TRA AFHDS3.h"
#include "USART.h"

typedef struct tSES_BoundConfig sSES_BoundConfig;
typedef struct tSES_Command sSES_Command;
typedef void tSES_BindCallback(BOOL Success);
typedef void tSES_HeartbeatCallback(void);
#if defined(ROLE_TX)
	typedef void tSES_RealtimePacketReceivedCallback(unsigned char Command,const void *pPayload,BOOL IsAuxiliaryReceiver);
#elif defined(ROLE_RX)
	typedef void tSES_RealtimePacketReceivedCallback(unsigned char Command,const void *pPayload);
#endif
#if defined(ROLE_RX)
typedef BOOL tSES_PreSetChannelsCallback(signed short *pChannels,BOOL IsFailsafe);
typedef void tSES_SetChannelsCallback(unsigned long CallbackValue);
#endif
// Send to the upper layer some data received from the other device auxiliary transparent serial communication service
typedef BOOL tSES_WSDataReceivedCallback(const void *pData,unsigned long DataLength);
typedef void tSES_SendCommandCallback(sSES_Command *pCommand);
typedef void tSES_CommandReceivedCallback(unsigned short Code,const void *pArgument,unsigned long ArgumentLength);
// Return the real-time packet command code
typedef unsigned char tSES_BuildRealtimePacketCallback(void *pPayload);

#define RF_LIBRARY_VERSION 0x0100

// Values of real-time packet commands, must be above or equal to 0x20
#define SES_CMD_CHANNELS      0x20
#define SES_CMD_FAILSAFE      0x21
#define SES_CMD_SENSORS       0x22 // Sent only by 1-way or main receiver
#define SES_CMD_SENSORS_AUX   0x23 // This kind of packet is sent only by an auxiliary receiver and an auxiliary receiver may send only this kind of packet
// These packets are processed by 1-way and auxiliary receivers only
#define SES_CMD_V0_CONFIG_1   0x30 // Argument of sSES_RTPA_ReceiverV0Config type, processed only by one-way V0 receivers
#define SES_CMD_V1_CONFIG_1   0x30 // Argument of sSES_RTPA_ReceiverV1Config type, processed only by one-way or auxiliary V1 receivers
#define SES_CMD_SVC           0x31 // Argument of sSES_CA_SVC type, processed only by one-way or auxiliary V1 receivers
#define SES_CMD_PWM_FREQUENCY 0x32 // Argument of sSES_RTPA_PWMFrequency type, processed only by one-way or auxiliary V1 receivers
#define SES_CMD_SHORT_COMMAND 0x3E // Argument of sSES_RTPA_ShortCommand type, Allows sending commands with argument <=5 bytes, no answer
#define SES_CMD_NONE          0x3F // Used internally or when no real-time packet is available

#define SES_MIN_TX_PAYLOAD            8
#define SES_MAX_CMD_ARGUMENT_LENGTH  32
#define SES_MAX_EXTERNAL_DATA_LENGTH 32

#define SES_REALTIME_CONFIG_PACKET_INTERVAL_MS 1000

#define SES_MAX_PAYLOAD_LENGTH       (1+SES_MAX_CHANNELS_LENGTH)

// Command codes
#define SES_CC_CR_MASK        0x8000 // Command / response mask
#define SES_CC_COMMAND        0x0000 // Command
#define SES_CC_RESPONSE       0x8000 // Response to a previous command
#define SES_CC_LR_MASK        0x4000 // Local / remote mask
#define SES_CC_LOCAL          0x0000 // Command handled locally
#define SES_CC_REMOTE         0x4000 // Command handled remotely
#define SES_CC_SRC_SA_MASK    0x2000 // Source session / application mask
#define SES_CC_SRC_SESSION    0x0000 // Command sent by the session layer
#define SES_CC_SRC_APP        0x2000 // Command sent by the host application
#define SES_CC_DST_SA_MASK    0x1000 // Destination session / application mask
#define SES_CC_DST_SESSION    0x0000 // Command received by the session layer
#define SES_CC_DST_APP        0x1000 // Command received by the host application
#define SES_CC_CODE_MASK      0x0FFF // Command code mask

// Response that indicates that a command is not supported by the recipient
// Handling: local or remote
// Source: application or session
// Destination: application session
#define SES_CC_COMMAND_NOT_SUPPORTED 0x0001
typedef struct __attribute__((packed))
{
	unsigned short CommandCode;
} sSES_CA_CommandNotSupportedResponse;

// Response that indicates that a command is invalid
// Handling: local or remote
// Source: application or session
// Destination: application session
#define SES_CC_COMMAND_INVALID 0x0002
typedef struct __attribute__((packed))
{
	unsigned short CommandCode;
} sSES_CA_CommandInvalidResponse;

// Set the type of the 32 channels
// Handling: remote
// Source: session, when binding only (transmitter only)
// Destination: session (receiver only)
#define SES_CC_SET_CHANNELS_TYPE 0x0010
typedef struct __attribute__((packed))
{
	unsigned char ChannelsType[SES_CHANNELS_TYPE_LENGTH]; // 5 bits per channel from the values defined in SES_CT_*
} sSES_CA_SetChannelsType;

// Set the failsafe
// Handling: local or remote
// Source: application or session when binding only
// Destination: session, application is notified (receiver only)
#define SES_CC_SET_FAILSAFE 0x0011

// Set the failsafe timeout
// Handling: local or remote
// Source: application or session when binding only
// Destination: session, application is notified (receiver only)
#define SES_CC_SET_FAILSAFE_TIMEOUT 0x0012
typedef struct __attribute__((packed))
{
	unsigned short Timeout;
} sSES_CA_SetFailsafeTimeout;

// Set the RF power
// Handling: local or remote
// Source: application
// Destination: session, application is notified
#define SES_CC_SET_RF_POWER 0x0013
typedef struct __attribute__((packed))
{
	signed short Power; // Power used when running in unit of 0.25dBm
} sSES_CA_SetRFPower;

// Get the RF power
// Handling: local or remote
// Source: application
// Destination: session
#define SES_CC_GET_RF_POWER 0x0014
typedef struct __attribute__((packed))
{
	signed short Power; // Power used when running in unit of 0.25dBm
} sSES_CA_GetRFPowerResponse;

// Get receiver capabilities
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_GET_CAPABILITIES 0x0015
typedef struct __attribute__((packed))
{
	unsigned char HasTwoAntennas:1;
	unsigned char HasPWMOutputs:1;
	unsigned char HasPPMOutput:1;
	unsigned char HasExternalWSPort:1;
	unsigned char SupportsIBus1:1;
	unsigned char SupportsIBus2:1;
	unsigned char SupportsSBus:1;
	unsigned char HasDualExternalBusPorts:1;

	unsigned char HasDualExternalBusUSARTs:1;
	unsigned char HasSVC:1;
	unsigned char Reserved1:6;

	unsigned char Reserved2[32-2]; // 256 bits for 256 capabilities
} sSES_CA_GetCapabilitiesResponseV0;
typedef struct __attribute__((packed))
{
	unsigned char NbRCChannels:5;
	unsigned char NbNewPortPorts:3; // From 0 to 4 ports

	unsigned char HasTwoAntennas:1;
	unsigned char HasSVC:1;
	unsigned char HasRXFlightController:1;
    unsigned char HasAdvancedSVC:1;
	unsigned char Reserved1:4;

	unsigned char Reserved2[32-2]; // 256 bits for 256 capabilities
} sSES_CA_GetCapabilitiesResponseV1;

// Set the type of analog output used on the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_ANALOG_OUTPUT 0x0016
typedef enum __attribute__((packed))
{
	SES_ANALOG_OUTPUT_PWM,
	SES_ANALOG_OUTPUT_PPM
} eSES_PA_SetAnalogOutput;

// Set the PWM output frequency on the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_PWM_FREQUENCY_V0 0x0017
// The argument is a sSES_PWMFrequencyV0 structure defined in  "SES ADHDS3 external.h"

// Set the type of external bus used on the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_EXTERNAL_BUS_TYPE 0x0018
typedef struct __attribute__((packed))
{
	eEB_BusType Type;
} sSES_CA_SetExternalBusType;

// Start or stop the setup of an i-Bus output servo
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_IBUS_SETUP_SERVO 0x0019
typedef struct __attribute__((packed))
{
	unsigned char ChannelNb; // 0xFF to stop
} sSES_CA_IBusSetupServo;
typedef struct __attribute__((packed))
{
	unsigned char ChannelNb; // 0xFF when stopping
	unsigned char ID; // 0 if no button pressed
	unsigned char OutputNb;
} sSES_CA_IBusSetupServoResponse;

// Notify the application of a change in the signal status
// Handling: local
// Source: session
// Destination: application notified as a session command
#define SES_CC_SIGNAL_STATUS_CHANGE 0x001A
typedef struct __attribute__((packed))
{
	eSES_SignalStatus Status;
} eSES_PA_SignalStatus;

// Indicates the supported sensors by the transmitter to the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_SUPPORTED_SENSORS 0x001B
typedef struct __attribute__((packed))
{
	unsigned char Types[IB_NB_DEVICE_TYPES/8]; // 1 bit per sensor (device type)
} sSES_CA_SetSupportedSensors;

// Set the remote device to test mode
// The link will be lost after this command is sent
// Handling: remote
// Source: application
// Destination: application
#define SES_CC_TEST 0x001C
typedef struct __attribute__((packed))
{
	sLNK_Test Test;
} sSES_CA_Test;

// Restart the remote device in firmware upgrade mode
// Handling: remote
// Source: application
// Destination: application
#define SES_CC_UPGRADE_FIRMWARE 0x001D

// Set SVC (Smart Vehicle Control)
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_SVC 0x001E
typedef struct __attribute__((packed))
{
	unsigned char SteeringGain; // From 0 to 100%
	unsigned char ThrottleGain; // From 0 to 100%
	unsigned char Priority; // From 0 to 100
	unsigned char Neutral; // From 0 to 100% (50% is centered)
	unsigned char EndpointLow; // From 0 to 150%
	unsigned char EndpointHigh; // From 0 to 150%
	unsigned char GyroReverse:1;
	unsigned char ThrottleReverse:1; // 0=normal, 1=reversed
	unsigned char ESPMode:1; // 0=mode 1, 1=mode 2
	unsigned char Reserved:4;
	unsigned char SVCOn:1; // 1 if SVC is active, 0 if SVC is disabled
} sSES_CA_SVC;

// Read product number and hardware and firmware version
// Handling: remote
// Source: application
// Destination: application
#define SES_CC_GET_VERSION 0x001F
typedef struct __attribute__((packed))
{
	unsigned long ProductNumber;
	unsigned short MainboardVersion;
	unsigned short RFModuleVersion;
	unsigned short BootloaderVersion;
	unsigned short FirmwareVersion;
	unsigned short RFLibraryVersion;
} sSES_CA_GetVersionResponse;

// Set the direction of the single bus port
// Available only in receivers having the SES_CAP_SINGLE_BUS_PORT capability
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_SINGLE_BUS_PORT_TYPE 0x0020
typedef struct __attribute__((packed))
{
	eEB_SingleBusPortType Type;
} sSES_PA_SetSingleBusPortType;

// Set an i-Bus 1 peripheral parameter
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_IBUS1_SET_PARAM 0x0021
typedef struct __attribute__((packed))
{
	unsigned char ID;
	unsigned short ParamType;
	union __attribute__((packed))
	{
		unsigned char UChar;
		signed char SChar;
		unsigned short UShort;
		signed short SShort;
		unsigned long ULong;
		signed long SLong;
	} ParamValue;
} sSES_CA_IBus1SetParam;

typedef struct __attribute__((packed))
{
	unsigned char ID;
	unsigned short ParamType;
} sSES_CA_IBus1SetParamResponse;

// Get an i-Bus 1 peripheral parameter
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_IBUS1_GET_PARAM 0x0022
typedef struct __attribute__((packed))
{
	unsigned char ID;
	unsigned short ParamType;
} sSES_CA_IBus1GetParam;

typedef struct __attribute__((packed))
{
	unsigned char ID;
	unsigned short ParamType;
	union __attribute__((packed))
	{
		unsigned char UChar;
		signed char SChar;
		unsigned short UShort;
		signed short SShort;
		unsigned long ULong;
		signed long SLong;
	} ParamValue;
} sSES_CA_IBus1GetParamResponse;

// Configure the WS external USART of the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_WS_USART_CONFIG 0x0023
typedef struct __attribute__((packed))
{
	sUSART_Config Config;
} sSES_CA_SetWSUSARTConfig;

// Select the antenna to be used
// Handling: local or remote
// Source: application
// Destination: session
#define SES_CC_SET_ANTENNA 0x0024
typedef struct __attribute__((packed))
{
	eLNK_Antenna Antenna;
} sSES_CA_SetAntenna;

// Set an i-Bus 2 peripheral parameter
// Handling: remote
// Source: application
// Destination: application
#define SES_CC_IBUS2_SET_PARAM 0x0025
// sSES_CA_IBus2SetParam defined in "SES ADHDS3 external.h"
// sSES_CA_IBus2SetParamResponse defined in "SES ADHDS3 external.h"

// Get an i-Bus peripheral parameter
// Handling: remote
// Source: application
// Destination: application
#define SES_CC_IBUS2_GET_PARAM 0x0026
// sSES_CA_IBus2GetParam defined in "SES ADHDS3 external.h"
// sSES_CA_IBus2GetParamResponse defined in "SES ADHDS3 external.h"

#if AFHDS3_VERSION==1
// Set the type of the New Port interfaces used on the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_NEW_PORT_TYPE 0x0027
typedef struct __attribute__((packed))
{
	eSES_NewPortType Types[SES_NPT_NB_MAX_PORTS];
} sSES_CA_SetNewPortTypes;
#endif

#if AFHDS3_VERSION==1
// Set the PWM output frequency on the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_PWM_FREQUENCIES_V1 0x0028
typedef struct __attribute__((packed))
{
	BOOL LowHigh; // 0=channels 1 to 16, 1=channels 17 to 32
	sSES_PWMFrequenciesHalfV1 Frequencies;
} sSES_CA_SetPWMFrequenciesV1;
#endif

#if AFHDS3_VERSION==1
// Set the number of the RC channel to output on the first PWM output of the receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_RX_FIRST_PWM_RC_CHANNEL_NB 0x0029
typedef struct __attribute__((packed))
{
	unsigned char ChannelNb;
} sSES_CA_SetRXFirstPWMRCChannelNb;
#endif

// Defines if the failsafe, when triggered, output only low level (TRUE) or defined channel values
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_IS_FAILSAFE_IN_NO_OUTPUT_MODE 0x002A
typedef struct __attribute__((packed))
{
	BOOL IsFailsafeInNoOutputMode;
} sSES_CA_SetIsFailsafeInNoOutputMode;

// Set the RC channel outputting the signal strength
// Handling: remote
// Source: application (transmitter only)
// Destination: session, application is notified (receiver only)
#define SES_CC_SET_SIGNAL_STRENGTH_RC_CHANNEL_NB 0x002B
typedef struct __attribute__((packed))
{
	unsigned char ChannelNb; // 0xFF if the function is not used
} sSES_CA_SetSignalStrengthRCChannelNb;

// Calibrate the V0 receiver's voltage monitor
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_CALIBRATE_VOLTAGE_MONITOR_V0 0x002C
typedef struct __attribute__((packed))
{
	unsigned long ActualInternalVoltage; // Voltage currently supplied to the receiver in unit of 1mV
} sSES_CA_CalibrateVoltageMonitorV0;
typedef struct __attribute__((packed))
{
	unsigned short InternalVoltageCorrection; // 1<<14=1.0
} sSES_CA_CalibrateVoltageMonitorV0Response;

#if AFHDS3_VERSION==1
// Calibrate the V1 receiver's voltage monitors
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_CALIBRATE_VOLTAGE_MONITOR_V1 0x002C
typedef struct __attribute__((packed))
{
	unsigned long ActualInternalVoltage; // Voltage currently supplied to the receiver in unit of 1mV, zero if no calibration needed
	unsigned long ActualExternalVoltage; // External voltage currently measured by the receiver in unit of 1mV, zero if no calibration needed
} sSES_CA_CalibrateVoltageMonitorV1;
typedef struct __attribute__((packed))
{
	unsigned short InternalVoltageCorrection; // 1<<14=1.0
	unsigned short ExternalVoltageCorrection; // 1<<14=1.0
} sSES_CA_CalibrateVoltageMonitorV1Response;
#endif

#if AFHDS3_VERSION==1
// Start SVC calibration (Smart Vehicle Control calibration)
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_CALIBRATE_SVC 0x002D
typedef struct __attribute__((packed))
{
	unsigned char CalibrationType:7; // 0=SVC calibration, 1=gyroscope calibration
	unsigned char StartCalibration:1; // 1=start calibration, 0=check status
} sSES_CA_CalibrateSVC;
typedef enum
{
	SES_CSVCS_NO_CALIBRATION, // No calibration has been initiated
	SES_CSVCS_IN_PROGRESS,    // Calibration in progress
	SES_CSVCS_SUCCESSFUL,     // Calibration successful
	SES_CSVCS_FAILED,         // Calibration failed
} eSES_CA_CalibrateSVCStatus;
typedef struct __attribute__((packed))
{
	eSES_CA_CalibrateSVCStatus Status;
} sSES_CA_CalibrateSVCResponse;
#endif

#if AFHDS3_VERSION==1
// Set a parameter of the internal flight controller of a V1 receiver
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_RX_FLIGHT_CONTROLLER_PARAM 0x002E
// The parameter of this command is as follow:
// 1 byte: parameter number
// 0 to 15 bytes: parameter value
// The answer of this command is as follow:
// 1 byte: error code (0=success)
// 0 to 15 bytes: parameter specific answer
// The structures of the commands and responses are defined in the "RX flight controller.h" header file
#endif


#if AFHDS3_VERSION==1
// Set Advanced SVC (Smart Vehicle Control)
// Handling: remote
// Source: application (transmitter only)
// Destination: application (receiver only)
#define SES_CC_SET_ADVANCED_SVC 0x002F
typedef struct __attribute__((packed))
{
    unsigned char TurningGain;    // From 0 to 100%
    unsigned char ForwardSpeed;   // From 0 to 100%
    unsigned char ReturnSpeed;    // From 0 to 100%
    unsigned char Filter;         // From 0 to 100%
    unsigned char CarType;        // 0=GeneralCars, 1=F1,2=DriftCars,3=DragRaceCars
    unsigned char Reserved[3];
} sSES_CA_ADVANCED_SVC;
#endif


// Structure sent by the SES_CMD_V0_CONFIG_1 real-time packet
typedef struct __attribute__((packed))
{
	sSES_PWMFrequencyV0 PWMFrequency;
	eEB_BusType ExternalBusType:4;
	eSES_PA_SetAnalogOutput AnalogOutput:2;
	unsigned char IsFailsafeInNoOutputMode:1;
	eEB_SingleBusPortType SingleBusPortType:1; // For single external bus port receivers only
	unsigned char Reserved2[5]; // Should be set to zero
} sSES_RTPA_ReceiverV0Config;

#if AFHDS3_VERSION==1
// Structure sent by the SES_CMD_V1_CONFIG_1 real-time packet
typedef struct __attribute__((packed))
{
	eSES_NewPortType NewPortTypes[SES_NPT_NB_MAX_PORTS];
	unsigned char RXFirstPWMRCChannelNb;
	unsigned char IsFailsafeInNoOutputMode:1;
	unsigned char Reserved1:7; // Should be set to zero
	unsigned char Reserved2[2]; // Should be set to zero
} sSES_RTPA_ReceiverV1Config;
#endif

#define SES_NB_REALTIME_PWM_FREQUENCY_CHANNELS 4
// Structure sent by the SES_CMD_PWM_FREQUENCY real-time packet
typedef struct __attribute__((packed))
{
	unsigned char FirstChannelNb;
	unsigned char PWMFrequencies[(SES_PWM_FREQUENCY_V1_NB_BITS*SES_NB_REALTIME_PWM_FREQUENCY_CHANNELS+7)/8]; // 9 bits per channels, 4 channels total
	unsigned char Synchronized; // 1 bit per channel, 4 channels total
	unsigned char Reserved; // Should be set to zero
} sSES_RTPA_PWMFrequencies;

#define SES_MAX_SHORT_CMD_ARGUMENT_LENGTH 5
// Structure sent by the SES_CMD_SHORT_COMMAND real-time packet
typedef struct __attribute__((packed))
{
	unsigned short Code;
	unsigned char ArgumentLength; // From 0 to SES_MAX_SHORT_CMD_ARGUMENT_LENGTH
	unsigned char Argument[SES_MAX_SHORT_CMD_ARGUMENT_LENGTH];
} sSES_RTPA_ShortCommand;






// Enable or disable the feedback from the receiver
// WS service will stop if the feedback is disabled
// Handling: remote
// Source: application
// Destination: session
#define SES_CC_SET_FEEDBACK 0x0888
typedef enum __attribute__((packed))
{
	SES_PA_FEEDBACK_ON,
	SES_PA_FEEDBACK_OFF
} eSES_PA_SetFeedback;






typedef enum tSES_CommandResult
{
	SES_CR_SUCCESS,
	SES_CR_TIMEOUT,
	SES_CR_NOT_SUPPORTED,
	SES_CR_INVALID
} eSES_CommandResult;

#define SES_SEND_COMMAND_DUMMY_CALLBACK ((tSES_SendCommandCallback *)1)
typedef struct tSES_Command
{
	unsigned short Code;
	unsigned char ArgumentLength; // From 0 to SES_MAX_CMD_ARGUMENT_LENGTH
	unsigned char ResponseLength; // From 0 to SES_MAX_CMD_ARGUMENT_LENGTH
	tSES_SendCommandCallback *pCallback;
	unsigned long Timeout; // In milliseconds
	unsigned char Argument[SES_MAX_CMD_ARGUMENT_LENGTH];
	unsigned char Response[SES_MAX_CMD_ARGUMENT_LENGTH];
	eSES_CommandResult Result;
	unsigned char Padding[3];
} sSES_Command;

typedef struct tSES_Run sSES_Run;
typedef struct
{
	tSES_BindCallback *pBindCallback;
	const sSES_Run *pSessionRun;
	sTRA_BindApp TransportBindApp;
} sSES_Bind;

typedef struct tSES_BoundConfig
{
	unsigned char ChannelsType[SES_CHANNELS_TYPE_LENGTH]; // 5 bits per channel from the values defined in SES_CT_*
	unsigned char FailsafePacked[SES_MAX_CHANNELS_LENGTH];
	unsigned short FailsafeTimeout;
	unsigned char SignalStrengthRCChannelNb; // 0xFF if not used
	unsigned char Padding;
	sTRA_BoundConfig TransportBoundConfig;
} sSES_BoundConfig;

// Structure supplied by the application
typedef struct
{
	sTRA_FactoryApp TransportFactory;
} sSES_FactoryApp;

typedef struct
{
	sTRA_Init TransportInit;
} sSES_Init;

typedef struct tSES_Run
{
	tSES_RealtimePacketReceivedCallback *pRealtimePacketReceivedCallback;
	tSES_WSDataReceivedCallback *pWSDataReceivedCallback;
	tSES_CommandReceivedCallback *pCommandReceivedCallback;
#if defined(ROLE_RX)
	tSES_PreSetChannelsCallback *pPreSetChannelsCallback;
	tSES_SetChannelsCallback *pSetChannelsCallback;
#endif
	tSES_BuildRealtimePacketCallback *pBuildRealtimePacketCallback;
	tSES_HeartbeatCallback *pHeartbeatCallback;
	sSES_BoundConfig *pBoundConfig;
#if defined(ROLE_RX)
	unsigned char *pChannelsPacked;
	signed short *pChannels;
	unsigned char *pUsedFailsafePacked;
	signed short *pUsedFailsafe;
#endif
	const sTRA_RunApp *pTransportRunApp;
} sSES_Run;

#if defined(ROLE_TX)
typedef struct
{
	sTRA_SpectrumAnalyzerApp Transport;
} sSES_SpectrumAnalyzerApp;
#endif

typedef struct
{
	sTRA_Test TransportTest;
} sSES_Test;

#if defined(ROLE_TX)
typedef struct
{
	sTRA_UpdateApp TransportUpdateApp;
} sSES_UpdateApp;
#endif

void SES_MsIRQHandler(void);
void SES_Bind(const sSES_Bind *pBind);
void SES_Factory(const sSES_FactoryApp *pFactoryApp);
unsigned long SES_GetFreeWSTXBufferLength(void);
#if defined(ROLE_RX)
	void SES_GetLastRXConditions(sLNK_RXConditions *pRXConditions);
#endif
unsigned long SES_GetRFCycleTime(void);
unsigned long SES_GetUsedWSTXBufferLength(void);
unsigned short SES_GetVersion(void);
void SES_Idle(void);
void SES_Init(const sSES_Init *pInit);
BOOL SES_IsWSIdle(void);
void SES_Run(const sSES_Run *pRun);
BOOL SES_SendCommand(sSES_Command *pCommand);
void SES_SendResponse(unsigned short Code,const void *pArgument,unsigned long ArgumentLength);
unsigned long SES_SendWSData(const void *pData,unsigned long DataLength);
#if defined(ROLE_TX)
	unsigned long SES_SpectrumAnalyzer(sSES_SpectrumAnalyzerApp *pSpectrumAnalyzerApp);
#endif
void SES_Stop(void);
void SES_Test(sSES_Test *pTest);
#if defined(ROLE_TX)
	void SES_Update(const sSES_UpdateApp *pUpdateApp);
#endif

#endif // !defined(SES_AFHDS3_H)
