#if !defined(AFHDS3_PWM_FREQUENCIES_PACKING_LIBRARY_H)
#define AFHDS3_PWM_FREQUENCIES_PACKING_LIBRARY_H

#include "System definitions.h"

#include "SES AFHDS3 external.h"

void SES_PackPWMFrequencies(const sSES_PWMFrequenciesAPPV1 *pUnpackedPWMFrequencies,sSES_PWMFrequenciesHalfV1 *pPackedPWMFrequencies,BOOL LowHigh);
void SES_UnpackPWMFrequencies(const sSES_PWMFrequenciesHalfV1 *pPackedPWMFrequencies,sSES_PWMFrequenciesAPPV1 *pUnpackedPWMFrequencies,BOOL LowHigh);

#endif // !defined(AFHDS3_PWM_FREQUENCIES_PACKING_LIBRARY_H)
