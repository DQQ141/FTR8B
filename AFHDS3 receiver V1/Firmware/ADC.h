#if !defined(ADC_H)
#define ADC_H

#define ADC_NB_SAMPLES 32

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8)
	#define ADC_NB_CHANNELS                  2
#elif defined(PN_FBR12) || defined(PN_FBR4)
	#define ADC_NB_CHANNELS                  1
	#define ADC_BATTERY_MONITOR_CHANNEL_NB   6
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D) 
	#define ADC_NB_CHANNELS                  2
#elif defined(PN_TMR)
	#define ADC_NB_CHANNELS                  1
	#define ADC_BATTERY_MONITOR_CHANNEL_NB   9
#elif defined(PN_GMR)
	#define ADC_NB_CHANNELS                  1
	#define ADC_BATTERY_MONITOR_CHANNEL_NB   1
#endif

void ADC_MsIRQHandler(void);
unsigned long ADC_GetInternalVoltage(void);
#if ADC_NB_CHANNELS!=1
	unsigned long ADC_GetExternalVoltage(void);
#endif
void ADC_Init(void);

#endif // !defined(ADC_H)
