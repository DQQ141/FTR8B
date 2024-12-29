#if !defined(MAIN_H)
#define MAIN_H

#include "System definitions.h"

#include "i-Bus 1 device library.h"
#include "i-Bus 2 device library.h"
#include "LED.h"
#include "New Port.h"
#include "SES AFHDS3.h"
#include "USART.h"

#define APP_EEPPROM_SIZE 0x200

#define APP_BIND_SIGNATURE_1 0xC1B478FE
#define APP_BIND_SIGNATURE_2 0x88210D49

#if defined(PN_TR8B)
#define APP_MODE_WAITSYNCHRONIZED_TIME    (2000) //ms
#define APP_MODE_WAITBIND_TIME            (3000)//(10000)//ms
typedef enum
{
	APP_MODE_NONE,
	APP_MODE_RUN,
	APP_MODE_WAITSYNCHRONIZED,
	APP_MODE_BIND,
	APP_MODE_WAITBIND	
} eAPP_ModeStatus;
#endif
typedef struct
{
	sSES_CA_SetSupportedSensors SupportedSensors;
	sSES_PWMFrequenciesHalfV1 PWMFrequencies[2];
	BOOL IsFailsafeInNoOutputMode;
	unsigned char RXFirstPWMRCChannelNb; // Number of the RC channel to output on the first PWM output of the receiver
	sUSART_Config WSUSARTConfig;
#if defined(PN_INR4_GYB) || defined(PN_GMR)
	sSES_CA_SVC SVC;
    sSES_CA_ADVANCED_SVC AdvancedSVC;
#else
	unsigned char Padding[3];
#endif
} sAPP_ConfigRF;

typedef struct
{
	unsigned char NoContent;
} sAPP_ConfigPWM;

typedef struct
{
	unsigned short Length;
	unsigned short FirmwareVersion;
	unsigned short FailsafeTimeout;
	unsigned short InternalBatteryMonitorCorrection; // Internal battery monitor ADC correction, 1<<14=1.0
	unsigned short ExternalBatteryMonitorCorrection; // External battery monitor ADC correction, 1<<14=1.0
	unsigned char Padding1[2];
	sSES_BoundConfig BoundConfig;
	eSES_NewPortType NewPortTypes[SES_NPT_NB_MAX_PORTS];
	union {
		sAPP_ConfigRF RF;
		sAPP_ConfigPWM PWM;
	} C;
	unsigned long CRC32;
} sAPP_Config;

extern sLNK_FactoryConfig LNK_FactoryConfig;

extern unsigned char APP_ChannelsPacked1[SES_MAX_CHANNELS_LENGTH];
extern signed short APP_Channels1[SES_NB_MAX_CHANNELS];
extern signed short APP_Channels2[SES_NB_MAX_CHANNELS];
extern eSES_SignalStatus APP_SignalStatus;
extern signed short APP_UsedFailsafe[SES_NB_MAX_CHANNELS];
extern unsigned char APP_UsedFailsafePacked[SES_MAX_CHANNELS_LENGTH];
extern sAPP_Config APP_Config;
extern BOOL APP_ConfigChanged;
extern sSES_PWMFrequenciesAPPV1 APP_PWMFrequenciesV1;
extern const sLED_Pattern LED_Bind;

#if defined(PN_TR8B)
extern eAPP_ModeStatus   APP_ModeStatus;
void APP_RFModeSwitch(void);
extern sAPP_Config APP_OldConfig;
#endif
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_FBR8) || defined(PN_FBR4)
	void EXTI4_15_IRQHandler(void);
#elif defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FGR4D)
	void EXTI4_15_IRQHandler(void);
#elif defined(PN_GMR)
	void EXTI0_1_IRQHandler(void);
	void EXTI2_3_IRQHandler(void);
#endif
#if SES_NPT_NB_PORTS!=0
	void IB1_GetParamResponse(unsigned char ID,unsigned short ParameterType,unsigned long ParameterLength,const void *pParameterValue);
	void IB1_SetParamResponse(unsigned char ID,unsigned short ParameterType);
	void IB1_SetupServoResponse(unsigned char ChannelNb,unsigned char ID,unsigned char OutputNb);
	void IB2_GetParamResponse(const sSES_CA_IBus2GetParamResponse *pResponse,unsigned long ParamLength);
	void IB2_SetParamResponse(const sSES_CA_IBus2SetParamResponse *pResponse);
#endif
BOOL NPT_GetInternalSensor(sNPT_Sensor *pSensor);
void PHY_SetAntenna(unsigned char AntennaNb);
void PHY_SetAttenuator(BOOL AttenuatorOn);
void PHY_SetPAMode(ePHY_PAMode PAMode);

void APP_SaveConfig(void);

#endif // !defined(MAIN_H)
