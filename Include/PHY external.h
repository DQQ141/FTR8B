#if !defined(PHY_EXTERNAL_H)
#define PHY_EXTERNAL_H

#include "System definitions.h"

#define PHY_SIGNAL_LEVEL_UNKNOWN     (0xFF*4) // In -0.25dBm unit
#define PHY_BACKGROUND_NOISE_UNKNOWN (0xFF*4) // In -0.25dBm unit
typedef struct
{
	unsigned long SignalLevel; // In -0.25dBm unit
	unsigned long BackgroundNoise; // In -0.25dBm unit
} sPHY_RXConditions;

#endif // !defined(PHY_EXTERNAL_H)
