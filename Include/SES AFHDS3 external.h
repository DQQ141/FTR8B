#if !defined(SES_AFHDS3_EXTERNAL_H)
#define SES_AFHDS3_EXTERNAL_H

#include "System definitions.h"

#include "i-Bus 2.h"

#define SES_NB_MAX_CHANNELS             32
#define SES_NB_BITS_PER_CHANNEL_TYPE     5
#define SES_CHANNEL_RANGE_100        16384 // Channel range for 100% endpoint
#define SES_CHANNEL_RANGE_150        ((SES_CHANNEL_RANGE_100*150)/100) // Channel range for 150% endpoint
#define SES_FAILSAFE_KEEP_CHANNEL    (-32768) // Keep outputing the last channel value
#define SES_FAILSAFE_STOP_CHANNEL    (-32767) // For 6-bit channels and more only, outputs low or high level depending on the interface
#define SES_MAX_CHANNELS_LENGTH         32
#define SES_CHANNELS_TYPE_LENGTH     ((SES_NB_MAX_CHANNELS*SES_NB_BITS_PER_CHANNEL_TYPE+7)/8)

#define SES_V1_NB_DEDICATED_PWM_FREQUENCIES 4

// Each channel can occupy from 0 to 13 bits (n)
// All channel values are signed ranging from -(2^(n-1)-1) to 2^(n-1)-1. 0 corresponds to a PWM output of 1500us (middle point)
// PWM Output can range from 1000 to 2000us (Max endpoint is 100%) or from 750 to 2250us (max endpoint is 150%)
// Channel types            Channel bits                     Range        endpoint    PWM resolution
#define SES_CT_OFF  0x00UL //                           0  Channel not used, outputs 1500us
#define SES_CT_02B  0x02UL //  2 bits                   2     -1 to    1       100%        500.000us
#define SES_CT_03BP 0x13UL //  3 bits plus              3     -3 to    3       150%        250.000us
#define SES_CT_04BP 0x14UL //  4 bits plus              4     -6 to    6       150%        125.000us
#define SES_CT_05BP 0x15UL //  5 bits plus              5    -15 to   15       150%         50.000us
#define SES_CT_06BP 0x16UL //  6 bits plus              6    -30 to   30       150%         25.000us
#define SES_CT_06B  0x06UL //  6 bits                   6    -25 to   25       100%         20.000us
#define SES_CT_07B  0x07UL //  7 bits                   7    -50 to   50       100%         10.000us
#define SES_CT_08BP 0x18UL //  8 bits plus              8    -75 to   75       150%         10.000us
#define SES_CT_08B  0x08UL //  8 bits                   8   -125 to  125       100%          4.000us
#define SES_CT_09BP 0x19UL //  9 bits plus              9   -150 to  150       150%          5.000us
#define SES_CT_09B  0x09UL //  9 bits                   9   -250 to  250       100%          2.000us
#define SES_CT_10BP 0x1AUL // 10 bits plus             10   -375 to  375       150%          2.000us
#define SES_CT_10B  0x0AUL // 10 bits                  10   -500 to  500       100%          1.000us
#define SES_CT_11BP 0x1BUL // 11 bits plus             11   -750 to  750       150%          1.000us
#define SES_CT_11B  0x0BUL // 11 bits                  11  -1000 to 1000       100%          0.500us
#define SES_CT_12BP 0x1CUL // 12 bits plus             12  -1500 to 1500       150%          0.500us
#define SES_CT_12B  0x0CUL // 12 bits                  12  -2000 to 2000       100%          0.250us
#define SES_CT_13BP 0x1DUL // 13 bits plus             13  -3000 to 3000       150%          0.250us
#define SES_CT_PLUS_MASK    0x10
#define SES_CT_NB_BITS_MASK 0x0F

#if AFHDS3_VERSION==1
#define SES_NPT_NB_MAX_PORTS 4
typedef enum
{
	SES_NPT_PWM,
	SES_NPT_PPM,
	SES_NPT_SBUS,
	SES_NPT_IBUS1_IN,
	SES_NPT_IBUS1_OUT,
	SES_NPT_IBUS2,
	SES_NPT_IBUS2_HUB_PORT,
	SES_NPT_WSTX,
	SES_NPT_WSRX,
	SES_NPT_NONE=0xFF
} eSES_NewPortType;
#endif

#define SES_PWM_FREQUENCY_MIN  50
#define SES_PWM_FREQUENCY_MAX 400
#define SES_NB_NARROW_PWM_STANDARDS 5
typedef struct
{
	unsigned short Period; // In unit of 0.25us
	unsigned short CenterValue; // In unit of 0.25us
	unsigned short ScaleFactor; // PulseWidth=((ChannelValue*ScaleFactor+(1<<15))>>16)+CenterValue in unit of 0.25us
} sSES_PWMStandard;

#if defined(SES_DEFINE_PWM_STANDARDS)
#define SES_PWM_MIN_PULSE_WIDTH 125 // In microseconds
static const sSES_PWMStandard SES_NarrowPWMStandards[SES_NB_NARROW_PWM_STANDARDS+1]={
	{2500*4,1500*4,2000*4}, // 2500us minimum period, 1500us center value, +/- 500us swing for 100% endpoint (Default standard)
	{1000*4, 500*4,1000*4}, // 1000us         period,  500us center value, +/- 250us swing for 100% endpoint (Narrow standard 1)
	{1200*4, 750*4,1000*4}, // 1200us         period,  750us center value, +/- 250us swing for 100% endpoint (Narrow standard 2)
	{1300*4, 300*4,1000*4}, // 1300us         period,  300us center value, +/- 250us swing for 100% endpoint (Narrow standard 3)
	{1788*4, 750*4,1000*4}, // 1788us         period,  750us center value, +/- 250us swing for 100% endpoint (Narrow standard 4)
	{1788*4, 500*4,1000*4}, // 1788us         period,  500us center value, +/- 250us swing for 100% endpoint (Narrow standard 5)
};
#endif

typedef struct __attribute__((packed))
{
	uIB2_ID ID;
	unsigned short ParamType;
} sSES_CA_IBus2GetParam;

typedef struct __attribute__((packed))
{
	uIB2_ID ID;
	unsigned short ParamType;
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH];
} sSES_CA_IBus2GetParamResponse;

typedef struct __attribute__((packed))
{
	uIB2_ID ID;
	unsigned short ParamType;
	unsigned char ParamValue[EB2_MAX_PARAM_LENGTH];
} sSES_CA_IBus2SetParam;

typedef struct __attribute__((packed))
{
	uIB2_ID ID;
	unsigned short ParamType;
	unsigned char ParamLength; // 0 if parameter not supported
} sSES_CA_IBus2SetParamResponse;

typedef struct __attribute__((packed))
{
	unsigned short Frequency:15; // From 50 to 400Hz
	unsigned short Synchronized:1; // 1=Synchronize the PWM output to the RF cycle (lower latency but unstable frequency)
} sSES_PWMFrequencyV0;

#define SES_PWM_FREQUENCY_V1_NB_BITS 9
// This structure store the PWM frequency and the synchronized state for 16 channels.
// 2 sets of this structure are required (LOW and HIGH) to define PWM parameters for channels 1 to 16 and 17 to 32.
// Having a half structure allows it to fit in a WS command that is limited to 32 bytes
typedef struct __attribute__((packed))
{
	unsigned char PWMFrequencies[(SES_PWM_FREQUENCY_V1_NB_BITS*SES_NB_MAX_CHANNELS/2+7)/8]; // 9 bits per channels, 16 channels total
	unsigned short Synchronized; // 1 bit per channel, 16 channels total
} sSES_PWMFrequenciesHalfV1;

// This structure may be used by main applications to store the PWM parameters in a single convenient structure
typedef struct __attribute__((packed))
{
	unsigned short PWMFrequencies[SES_NB_MAX_CHANNELS]; // One unsigned short per channel
	unsigned long Synchronized; // 1 bit per channel, 32 channels total
} sSES_PWMFrequenciesAPPV1;

typedef enum __attribute__((packed))
{
	SES_SS_SYNCHRONIZED,
	SES_SS_OUT_OF_SYNC,
	SES_SS_FAILSAFE
} eSES_SignalStatus;

unsigned long SES_SendWSData(const void *pData,unsigned long DataLength);

#endif // !defined(SES_AFHDS3_EXTERNAL_H)
