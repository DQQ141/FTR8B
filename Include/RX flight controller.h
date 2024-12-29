#if !defined(RX_FLIGHT_CONTROLLER_H)
#define RX_FLIGHT_CONTROLLER_H

// RX flight controller error codes
#define SES_RXFCEC_SUCCESS           0x00
#define SES_RXFCEC_GENERAL_FAILURE   0x01
#define SES_RXFCEC_INVALID_PARAMETER 0x02
#define SES_RXFCEC_NOT_SUPPORTED     0x03

// RX flight controller commands
// This structure is used for commands without parameter value
typedef struct __attribute__((packed))
{
	unsigned char ParamNb;
} sSES_RXFCC_GENERIC;

// RX flight controller responses
// This structure is used for responses without parameter value
typedef struct __attribute__((packed))
{
	unsigned char ErrorCode;
} sSES_RXFCR_GENERIC;

#endif // !defined(RX_FLIGHT_CONTROLLER_H)
