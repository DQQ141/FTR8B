#if !defined(FACTORY_LINK_H)
#define FACTORY_LINK_H

#include "System definitions.h"

#define LNK_FAC_BIND_TIMEOUT     100
#define LNK_FAC_FACTORY_TIMEOUT  500
#define LNK_FAC_PACKET_LENGTH     32

#define LNK_FACTORY_CHANNEL_NB (-40*1000*10) // 2440MHz

#define LNK_FAC_MEASURE_RSSI_TIME          100
#define LNK_FAC_MEASURE_RSSI_NB_AVERAGES    16
#define LNK_FAC_MEASURE_RSSI_FIRST_AVERAGE   5

#define LNK_FAC_LONG_RSSI_NB_AVERAGES 64

#define LNK_FAC_SENSITIVITY_NB_PACKETS 100

typedef enum
{
	FAC_LNK_CMD_FREQUENCY_OFFSET_CORRECTION=0x01,
	FAC_LNK_CMD_FREQUENCY_OFFSET_RESULT    =0x02,
	FAC_LNK_CMD_RSSI_CORRECTION            =0x03,
	FAC_LNK_CMD_RSSI_RESULT                =0x04,
	FAC_LNK_CMD_TX_POWER_CORRECTION        =0x05,
	FAC_LNK_CMD_TX_POWER_RESULT_LEFT       =0x06,
	FAC_LNK_CMD_TX_POWER_RESULT_RIGHT      =0x07,
	FAC_LNK_CMD_SENSITIVITY_LEFT           =0x08,
	FAC_LNK_CMD_SENSITIVITY_RIGHT          =0x09,
	FAC_LNK_CMD_BATTERY_MONITOR            =0x0A,
} eFAC_LNK_CommandCode;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	signed char FrequencyOffsetCorrection; // Frequency offset correction in 1Khz unit
} sFAC_LNK_CmdFrequencyOffsetResult;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	signed char RSSICorrection; // RSSI correction in unit of 0.25dB unit
} sFAC_LNK_CmdRSSIResult;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	signed char TXPowerCorrection; // TX power correction in unit of 0.25dB
} sFAC_LNK_CmdTXPowerResult;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	BOOL HighPower;
} sFAC_LNK_CmdSensitivity;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	unsigned char Padding[3];
	unsigned long RefInternalBatteryVoltage; // Voltage supplied to the UUT in unit of 1mV
#if AFHDS3_VERSION==1
	unsigned long RefExternalBatteryVoltage; // External voltage supplied to the UUT in unit of 1mV
#endif
} sFAC_LNK_CmdBatteryMonitor;

typedef union __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	unsigned char Raw[LNK_FAC_PACKET_LENGTH];
	sFAC_LNK_CmdFrequencyOffsetResult FrequencyOffsetResult;
	sFAC_LNK_CmdRSSIResult RSSIResult;
	sFAC_LNK_CmdTXPowerResult TXPowerResult;
	sFAC_LNK_CmdSensitivity Sensitivity;
	sFAC_LNK_CmdBatteryMonitor BatteryMonitor;
} uFAC_LNK_Command;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	unsigned short RSSI; // Expressed in unit of -0.25dBm, 0 if measurement in progress
} sFAC_LNK_RespRSSICorrectiontAndResult;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	unsigned char NbReceivedPackets; // 0 if measurement in progress
} sFAC_LNK_RespSensitivity;

typedef struct __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	signed short InternalBatteryVoltageCorrection; // Voltage correction in unit of 1mV
#if AFHDS3_VERSION==1
	signed short ExternalBatteryVoltageCorrection; // Voltage correction in unit of 1mV
#endif
} sFAC_LNK_RespBatteryMonitor;

typedef union __attribute__((packed))
{
	eFAC_LNK_CommandCode CommandCode;
	unsigned char Raw[LNK_FAC_PACKET_LENGTH];
	sFAC_LNK_RespRSSICorrectiontAndResult RSSICorrectiontAndResult;
	sFAC_LNK_RespSensitivity Sensitivity;
	sFAC_LNK_RespBatteryMonitor BatteryMonitor;
} uFAC_LNK_Response;

#endif // !defined(FACTORY_LINK_H)
