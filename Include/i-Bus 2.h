#if !defined(I_BUS_2_H)
#define I_BUS_2_H

#include "System definitions.h"

#define EB2_MAX_PARAM_LENGTH 16

#define IB2_USART_SPEED 1500000

// i-Bus 2 adresses are noted as AddressLevel2:AddressLevel1
// Address 7:7 is global broadcast
// Address 0:7 is the device directly connected to the receiver
// Address 7:n is the device connected to the output number n (from 0 to 6) of a hub directly connected to the receiver
// Address m:n is the device connected to the output number m (from 0 to 6) of a hub connected to the output number n (from 0 to 6)
//             of another hub directly connected to the receiver
// Adresses n:7 with n from 1 to 6 are reserved
#define IB2_ADDRESS_LOCAL 7

// i-Bus 2 parameters codes
// 
#define IB2PC_HUB_FUNCTION         0x0010
#define IB2_SET_HUB_FUNCTION_DELAY    200 // In milliseconds
// A hub device can fulfill the function of a hub or a PWM analog servos adapter.
// This parameter selects the function of a hub.
// The hub changes its type according to the argument of this parameter and resets itself to be re-enumerated by the receiver.
// This parameter is valid for IBDT_HUB_X_PORTS_HUB and IBDT_HUB_5_PORTS_PWM devices types with X being between 2 and 7.
// This parameter is of type eIB2_HubFunctions defined below
typedef enum
{
	IB2HF_HUB,
	IB2HF_PWM
} eIB2_HubFunctions;

typedef struct __attribute__((packed))
{
	unsigned char ChannelsType:1;
	unsigned char Failsafe:1;
	unsigned char ReceiverInternalSensors:1;
	unsigned char Reserved:5;
} sIB2_RequiredResourcesTypes;

typedef union __attribute__((packed))
{
	unsigned char All;
	sIB2_RequiredResourcesTypes Types;
} uIB2_RequiredResources;

#define IB2PC_PWM_ADAPTER_CHANNEL 0x0100 // +PortNb
// This parameter assigns a RC channel to a PWM port
// This parameter is valid for IBDT_HUB_X_PORTS_HUB and IBDT_HUB_5_PORTS_PWM devices types with X being between 2 and 7.
typedef struct __attribute__((packed))
{
	unsigned char ChannelNb;
} sIB2PA_PWMAdapterChannel;

#define IB2PC_PWM_ADAPTER_FREQUENCY 0x0200 // +PortNb
// This parameter selects the frequency of one PWM port and its synchronized state.
// This parameter is valid for IBDT_HUB_X_PORTS_HUB and IBDT_HUB_5_PORTS_PWM devices types with X being between 2 and 7.
typedef struct __attribute__((packed))
{
	unsigned short Frequency:15; // From 50 to 400Hz
	unsigned short Synchronized:1; // 1=Synchronize the PWM output to the RF cycle (lower latency but unstable frequency)
} sIB2PA_PWMAdapterFrequency;

#define IB2PC_RECEIVER_INTERNAL_SENSORS 0xC000
typedef struct __attribute__((packed))
{
	unsigned short InternalVoltage; // In 10mV units
	unsigned char SignalStrength; // From 0 to 100
	unsigned short RSSI; // In -0.25dBm units
	unsigned short BackgroundNoise; // In -0.25dBm units
	signed short SNR; // From 0 to 40dBm in 0.25dBm units
} sIB2PA_ReceiverInternalSensors;

typedef struct __attribute__((packed))
{
	unsigned char Reserved:7;
	unsigned char IsInternalSensor:1;
} sIB2_IDGeneric;

typedef struct __attribute__((packed))
{
	unsigned char SensorNumber:7;
	unsigned char IsInternalSensor:1;
} sIB2_IDInternalSensor;

typedef struct __attribute__((packed))
{
	unsigned char AddressLevel1:3;
	unsigned char AddressLevel2:3;
	unsigned char Reserved:1;
	unsigned char IsInternalSensor:1;
} sIB2_IDExternalDevice;

typedef union __attribute__((packed))
{
	unsigned char Byte;
	sIB2_IDGeneric Generic;
	sIB2_IDInternalSensor InternalSensor;
	sIB2_IDExternalDevice ExternalDevice;
} uIB2_ID;

#endif // !defined(I_BUS_2_H)
