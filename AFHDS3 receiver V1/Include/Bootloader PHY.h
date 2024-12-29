#if !defined(BOOTLOADER_PHY_H)
#define BOOTLOADER_PHY_H

#include "System definitions.h"

#if defined(PHY_SX1280)
	#include "Bootloader SX1280.h"
#endif
#include "Update.h"

#define PHY_RX_DURATION_INFINITE 0xFFFFFFFF

typedef enum
{
	PHY_PAM_IDLE,
	PHY_PAM_TX,
	PHY_PAM_RX
} ePHY_PAMode;

BOOL PHY_TimerCompareIRQHandler(void);
void PHY_Init(void);
void PHY_RX(sPHY_RXConfig *pRXConfig);
void PHY_TX(void);

void PHY_SetAntenna(unsigned char AntennaNb);
void PHY_SetAttenuator(BOOL AttenuatorOn);
void PHY_SetPAMode(ePHY_PAMode PAMode);

void BLL_BuildTXPacket(sLNK_UpdateResponse *pTXPacket);
void BLL_ProcessRXPacket(const sLNK_UpdateCommand *pRXPacket);

#endif // !defined(BOOTLOADER_PHY_H)
