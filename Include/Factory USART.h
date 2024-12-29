#if !defined(FACTORY_USART_H)
#define FACTORY_USART_H

#include "System definitions.h"

typedef enum
{
	FAC_USART_CMD_GET_VERSION =0x01,
	FAC_USART_CMD_CALIBRATE   =0x10,
	FAC_USART_CMD_TEST        =0x11,
	FAC_USART_CMD_TEST_NO_FAIL=0x12,
	FAC_USART_CMD_TEST_NO_CAL =0x13,
	FAC_USART_CMD_TEST_ZERO   =0x14,
	FAC_USART_CMD_CARRIER_ONLY=0x18,
	FAC_USART_CMD_STOP        =0x20,
} eFAC_USART_CommandCode;

typedef enum
{
	FAC_USART_RESP_GET_VERSION      =0x81,
	FAC_USART_RESP_START            =0x92,
	FAC_USART_RESP_STOP             =0x93,
	FAC_USART_RESP_FREQUENCY_OFFSET =0xA0,
	FAC_USART_RESP_RSSI             =0xA1,
	FAC_USART_RESP_TX_POWER         =0xA2,
	FAC_USART_RESP_SENSITIVITY      =0xA3,
	FAC_USART_RESP_BATTERY_MONITOR  =0xA4,
	FAC_USART_RESP_CARRIER_ONLY     =0xA8,
	FAC_USART_RESP_GENERAL_FAILURE  =0xAF,
} eFAC_USART_ResponseCode;

typedef struct __attribute__((packed))
{
	BOOL IsUUTATransmitter; // TRUE if UUT is a transmitter, FALSE if UUT is a receiver
	BOOL HasUUTTwoAntennas; // FALSE if UUT has only one antenna, TRUE if UUT has two antennas
	BOOL HasUUTTX; // FALSE if UUT can't transmit (normal use), TRUE if UUT is UUT can transmit
	BOOL HasUUTRX; // FALSE if UUT can't receive (normal use), TRUE if UUT is UUT can receive
	BOOL HasExternalBatteryMonitor; // TRUE if UUT has an external battery monitor, FALSE otherwise
	unsigned short RFAttenuationLow; // Test bench low RF attenuation in unit of 0.25dB
	unsigned short RFAttenuationHigh; // Test bench high RF attenuation in unit of 0.25dB
	signed short BindTXPower; // TX power used when binding in unit of 0.25dBm
	signed short FactoryTXPower; // TX power used when testing in unit of 0.25dBm
	signed short UUTBindTXPower; // TX power used by the UUT when binding in unit of 0.25dBm
	signed short UUTFactoryTXPower; // TX power used by the UUT in factory mode in unit of 0.25dBm
	unsigned short TimeoutBind; // Timeout in milliseconds when binding
	unsigned short TimeoutFrequencyOffset; // Timeout in milliseconds when testing the frequency offset
	unsigned short TimeoutRSSI; // Timeout in milliseconds when testing the RSSI
	unsigned short TimeoutTXPower; // Timeout in milliseconds when testing the TX power
	unsigned short TimeoutSensitivity; // Timeout in milliseconds when testing the sensitivity
	unsigned short TimeoutBatteryMonitor; // Timeout in milliseconds when testing the battery monitor
	unsigned char FrequencyOffsetCorrectionMargin; // Maximum frequency offset correction in unit of 1KHz
	unsigned char FrequencyOffsetResultMargin; // Maximum frequency offset error after correction in unit of 1KHz
	unsigned char FrequencyOffsetStep; // RSSI measurement step in KHz
	unsigned char FrequencyOffsetHop; // Frequency hop at each cycle in KHz
	unsigned short FrequencyOffsetMeasureRange; // Half range from the center frequency in KHz
	unsigned char FrequencyOffsetOffsetRange; // Maximum offset tested in KHz
	signed short RSSITXPower; // TX power used when testing UUT RSSI in unit of 0.25dB
	unsigned char RSSICorrectionMargin; // Maximum RSSI correction in unit of 0.25dB
	unsigned char RSSIResultMargin; // Maximum RSSI error after correction in unit of 0.25dB
	unsigned char TXPowerCorrectionMargin; // Maximum TX power correction in unit of 0.25dB
	unsigned char TXPowerResultMargin; // Maximum TX power error after correction in unit of 0.25dB
	signed short SensitivityTXPower; // Power used while testing the sensitivity in unit of 0.25dB unit
	unsigned long InternalBatteryVoltage; // Voltage suppried to the UUT in unit of 1mV
	unsigned long ExternalBatteryVoltage; // External voltage suppried to the UUT in unit of 1mV
	unsigned short InternalBatteryMonitorCorrectionMargin; // Maximum internal battery monitor correction in unit of 1mV
	unsigned short InternalBatteryMonitorCalibrationMargin; // Maximum internal battery monitor correction  when calibrating the test bench in unit of 1mV
	unsigned short ExternalBatteryMonitorCorrectionMargin; // Maximum external battery monitor correction in unit of 1mV
	unsigned short ExternalBatteryMonitorCalibrationMargin; // Maximum external battery monitor correction  when calibrating the test bench in unit of 1mV
} eFAC_USART_TestConditions;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_CommandCode CommandCode;
	unsigned char CRC8;
} sFAC_USART_CmdNoArgument;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_CommandCode CommandCode;
	eFAC_USART_TestConditions TestConditions;
	signed char RefUUTRSSICorrection; // Reference module RSSI correction in unit of 0.25dB
	signed char RefUUTTXPowerCorrection; // Reference module TX power correction in unit of 0.25dBm
	signed char RefUUTFrequencyOffset; // Reference module frequency offset in 1Khz unit
	signed short RefUUTInternalBatteryVoltageCorrection; // Reference voltage correction in unit of 1mV
	signed short RefUUTExternalBatteryVoltageCorrection; // Reference voltage correction in unit of 1mV
	unsigned char CRC8;
} sFAC_USART_CmdCalibrate;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_CommandCode CommandCode;
	signed short RFPower; // Carrier power in unit of 0.25dBm
	signed long ChannelNb; // If negative, represents the positive frequency offset from the beginning of the band in 100Hz increments
	unsigned char AntennaNb;
	BOOL ResetCalibration; // Zeros the calibration results of the testbench if TRUE
	unsigned char CRC8;
} sFAC_USART_CmdCarrierOnly;

typedef union __attribute__((packed))
{
	sFAC_USART_CmdNoArgument NoArgument;
	sFAC_USART_CmdCalibrate Calibrate;
	sFAC_USART_CmdCarrierOnly CmdCarrierOnly;
} uFAC_USART_Command;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	unsigned char CRC8;
} sFAC_USART_RespNoArgument;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	unsigned short Version; // 0x0100 = version 1.0
	unsigned long UUTProductCode;
	unsigned char CRC8;
} sFAC_USART_RespGetVersion;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	unsigned long UUT_ID;
	unsigned char CRC8;
} sFAC_USART_RespStart;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	signed char FrequencyOffsetCorrection; // Frequency offset correction in 1Khz unit
	BOOL FrequencyOffsetCorrectionSuccess;
	signed char FrequencyOffsetResult; // Frequency offset after correction in 1Khz unit
	BOOL FrequencyOffsetResultSuccess;
	unsigned char CRC8;
} sFAC_USART_RespFrequencyOffset;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	unsigned char RSSITestValue; // RSSI test value in -1dBm unit
	signed char RSSICorrection; // RSSI correction in 0.25dB unit
	BOOL RSSICorrectionSuccess;
	signed char RSSIResult; // RSSI result after correction in 0.25dB unit
	BOOL RSSIResultSuccess;
	unsigned char CRC8;
} sFAC_USART_RespRSSI;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	unsigned char TXPowerTargetValue; // TX power target value in 0.25dBm unit
	signed char TXPowerCorrection; // TX power correction on the left antenna in 0.25dB unit 
	BOOL TXPowerCorrectionSuccess;
	signed char TXPowerErrorResultLeft; // TX power error after correction on the left antenna in 0.25dB unit
	BOOL TXPowerErrorResultLeftSuccess;
	signed char TXPowerErrorResultRight; // TX power error after correction on the right antenna in 0.25dB unit
	BOOL TXPowerErrorResultRightSuccess;
	unsigned char CRC8;
} sFAC_USART_RespTXPower;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	unsigned char NbPacketsReceivedLeft; // Number of packets received on the left antenna
	BOOL NbPacketsReceivedLeftSuccess;
	unsigned char NbPacketsReceivedRight; // Number of packets received on the right antenna
	BOOL NbPacketsReceivedRightSuccess;
	unsigned char CRC8;
} sFAC_USART_RespSensitivity;

typedef struct __attribute__((packed))
{
	unsigned char Length;
	eFAC_USART_ResponseCode ResponseCode;
	signed short InternalBatteryVoltageCorrection; // Voltage correction in unit of 1mV
	BOOL InternalBatteryVoltageCorrectionSuccess;
	signed short ExternalBatteryVoltageCorrection; // Voltage correction in unit of 1mV
	BOOL ExternalBatteryVoltageCorrectionSuccess;
	unsigned char CRC8;
} sFAC_USART_RespBatteryMonitor;

typedef union __attribute__((packed))
{
	sFAC_USART_RespNoArgument NoArgument;
	sFAC_USART_RespGetVersion GetVersion;
	sFAC_USART_RespFrequencyOffset FrequencyOffset;
	sFAC_USART_RespRSSI RSSI;
	sFAC_USART_RespTXPower TXPower;
	sFAC_USART_RespSensitivity Sensitivity;
	sFAC_USART_RespBatteryMonitor BatteryMonitor;
} uFAC_USART_Response;

#endif // !defined(FACTORY_USART_H)

/*
SX1280 TX power -18 to 12dBm
Linear RSSI -10 to -90dBm
RSSI TX power -20 to -80dBm    
Sensitivity -100 to -110dBm    92dB
UUT TX power 5 to 20dBm        50dB
*/
