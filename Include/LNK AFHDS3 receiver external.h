#if !defined(LNK_AFHDS3_RECEIVER_EXTERNAL_H)
#define LNK_AFHDS3_RECEIVER_EXTERNAL_H

#include "System definitions.h"
#include "PHY external.h"

#define LNK_SIGNAL_STRENGTH_UNKNOWN 0xFF
#define LNK_SNR_UNKNOWN             (0xFF*4) // In -0.25dBm unit
typedef struct
{
	unsigned char SignalStrength; // From 0 (no signal) to 100 (perfect signal)
	unsigned char Padding[3];
	sPHY_RXConditions Phy;
} sLNK_RXConditions;

void LNK_GetLastRXConditions(sLNK_RXConditions *pRXConditions);

#endif // !define(LNK_AFHDS3_RECEIVER_EXTERNAL_H)
