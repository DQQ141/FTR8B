#if !defined(MAIN_PRIVATE_H)
#define MAIN_PRIVATE_H

#include "System definitions.h"

#define APP_BIND_SWITCH_TIME 3000

#define APP_SET_CHANNELS_MAX_TIME      40 // Maximum time in milliseconds between two calls to SES_SetChannelsCallback (not including RF small cycle time)
#define APP_SET_CHANNELS_MAX_RF_CYCLES 20 // Maximum number of small RF cycles between two calls to SES_SetChannelsCallback (not including RF small cycle time)

static void LNK_FactorySaveConfigCallback(void);
static void SES_BindCallback(BOOL Success);
static unsigned char SES_BuildRealtimePacketCallback(void *pPayload);
static void SES_CommandReceivedCallback(unsigned short Code,const void *pArgument,unsigned long ArgumentLength);
static void SES_CommandReceivedCallbackInvalid(unsigned short Code);
static void SES_CommandReceivedCallbackNotSupported(unsigned short Code);
static void SES_HeartbeatCallback(void);
static BOOL SES_PreSetChannelsCallback(signed short *pChannels,BOOL IsFailsafe);
static void SES_RealtimePacketReceivedCallback(unsigned char Command,const void *pPayload);
static void SES_SetChannelsCallback(unsigned long CallbackValue);
static BOOL SES_WSDataReceivedCallback(const void *pData,unsigned long DataLength);

static void APP_Background(void);
static void APP_Bind(void);
static BOOL APP_LoadConfig(void);
static void APP_LoadFactoryConfig(void);
static void APP_SaveFactoryConfig(void);
static void APP_SES_Run(BOOL CallSES_Run);
static void APP_StartNewPort(void);

int main(void);

#endif // !defined(MAIN_PRIVATE_H)
