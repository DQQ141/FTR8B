#if !defined(MAIN_H)
#define MAIN_H

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	void EXTI4_15_IRQHandler2(void);
#elif defined(PN_GMR)
	void EXTI0_1_IRQHandler2(void);
	void EXTI2_3_IRQHandler2(void);
#endif
void LNKTIM_TimerCompareIRQHandler(void);
void APP_BuildTXPacket(sLNK_UpdateResponse *pTXPacket);
void APP_ProcessRXPacket(const sLNK_UpdateCommand *pRXPacket);
int main(void);

#endif // !defined(MAIN_H)
