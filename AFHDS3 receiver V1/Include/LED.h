#if !defined(LED_H)
#define LED_H

#include "System definitions.h"

typedef struct
{
	unsigned long Pattern;
	unsigned short NbPatternBits;
	unsigned short MsPerPatternBit;
} sLED_Pattern;



extern const sLED_Pattern LED_Bind;
extern const sLED_Pattern LED_WaitBind;
extern const sLED_Pattern LED_Synchronizing;
extern const sLED_Pattern LED_Synchronized;

extern const sLED_Pattern LED_Test;
extern const sLED_Pattern LED_Factory;
extern const sLED_Pattern LEDP_InternalError;
extern const sLED_Pattern LED_PWMA_NoSignal;
extern const sLED_Pattern LED_PWMA_Configured;

extern const sLED_Pattern LED_SVC_Calibration;
extern const sLED_Pattern LED_FC_Calibration;

void LED_MsIRQHandler(void);
void LED_Init(void);
void LED_SetPattern(const sLED_Pattern *pPattern);

#endif // !defined(LED_H)
