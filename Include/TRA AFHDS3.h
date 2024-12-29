#if !defined(TRA_AFHDS3_H)
#define TRA_AFHDS3_H

#include "System definitions.h"
#include "LNK AFHDS3.h"

#define TRA_MAX_PAYLOAD_LENGTH 32

typedef struct tTRA_BoundConfig sTRA_BoundConfig;
typedef void tTRA_BindCallback(BOOL Success);
typedef void tTRA_HeartbeatCallback(void);
#if defined(ROLE_TX)
	typedef void tTRA_RealtimePacketReceivedCallback(unsigned char Command,const void *pPayload,BOOL IsAuxiliaryReceiver);
#elif defined(ROLE_RX)
	typedef void tTRA_RealtimePacketReceivedCallback(unsigned char Command,const void *pPayload);
#endif
// Send to the upper layer some data received from the other device transparent serial communication service
typedef BOOL tTRA_WSDataReceivedCallback(const void *pData,unsigned long DataLength);
// Return the real-time packet command code
typedef unsigned char tTRA_BuildRealtimePacketCallback(void *pPayload);

#define TRA_WS_RATIO_DENOMINATOR  8
typedef struct __attribute__((packed))
{
	unsigned char MaxWSRatio; // In 1/8, from 1 to 7
} sTRA_WSConfig;

typedef struct tTRA_RunApp sTRA_RunApp;
// Structure supplied by the application
typedef struct
{
	sLNK_BindApp LinkBindApp;
} sTRA_BindApp;

typedef struct tTRA_RunUp sTRA_RunUp;
// Structure supplied by the upper layer (session)
typedef struct
{
	tTRA_BindCallback *pBindCallback;
	const sTRA_RunApp *pTransportRunApp;
	const sTRA_RunUp *pTransportRunUp;
} sTRA_BindUp;

typedef struct tTRA_BoundConfig
{
	sTRA_WSConfig WSConfig;
	unsigned char TXPayloadLength; // Length of transmitter payload
	unsigned char RXPayloadLength; // Length of feedback (receiver) payload
	unsigned char Padding;
	uLNK_BoundConfig LinkBoundConfig;
} sTRA_BoundConfig;

// Structure supplied by the application
typedef struct
{
	sLNK_FactoryApp LinkFactory;
} sTRA_FactoryApp;

typedef struct
{
	sLNK_Init LinkInit;
} sTRA_Init;

// Structure supplied by the application
typedef struct tTRA_RunApp
{
	unsigned char *pWSTXBuffer;
	unsigned long WSTXBufferSize;
	sLNK_RunApp LinkRunApp;
} sTRA_RunApp;

// Structure supplied by the upper layer (session)
typedef struct tTRA_RunUp
{
	tTRA_RealtimePacketReceivedCallback *pRealtimePacketReceivedCallback;
	tTRA_WSDataReceivedCallback *pWSDataReceivedCallback;
	tTRA_BuildRealtimePacketCallback *pBuildRealtimePacketCallback;
	tTRA_HeartbeatCallback *pHeartbeatCallback;
	sTRA_BoundConfig *pBoundConfig;
} sTRA_RunUp;

#if defined(ROLE_TX)
typedef struct
{
	sLNK_SpectrumAnalyzerApp Link;
} sTRA_SpectrumAnalyzerApp;
#endif

typedef struct
{
	sLNK_Test LinkTest;
} sTRA_Test;

#if defined(ROLE_TX)
typedef struct
{
	sLNK_UpdateApp LinkUpdateApp;
} sTRA_UpdateApp;
#endif

void TRA_Bind(const sTRA_BindApp *pBindApp,const sTRA_BindUp *pBindUp);
void TRA_Factory(const sTRA_FactoryApp *pFactoryApp);
unsigned long TRA_GetFreeWSTXBufferLength(void);
unsigned long TRA_GetRFCycleTime(void);
#if defined(ROLE_RX)
	void TRA_GetLastRXConditions(sLNK_RXConditions *pRXConditions);
#endif
unsigned long TRA_GetUsedWSTXBufferLength(void);
void TRA_Idle(void);
void TRA_Init(const sTRA_Init *pInit);
BOOL TRA_IsWSIdle(void);
void TRA_Run(const sTRA_RunApp *pRunApp,const sTRA_RunUp *pRunUp);
BOOL TRA_SendWSData(const void *pData,unsigned long DataLength);
void TRA_SetAntenna(eLNK_Antenna Antenna);
#if defined(ROLE_TX)
	unsigned long TRA_SpectrumAnalyzer(sTRA_SpectrumAnalyzerApp *pSpectrumAnalyzerApp);
#endif
void TRA_Stop(void);
void TRA_Test(sTRA_Test *pTest);
#if defined(ROLE_TX)
	void TRA_Update(const sTRA_UpdateApp *pUpdateApp);
#endif

#endif // !defined(TRA_AFHDS3_H)
