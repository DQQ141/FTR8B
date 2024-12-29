#include "Poweronch.h"

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FGR12B) || defined(PN_FBR12) || defined(PN_INR4_GYB) || defined(PN_INR6_HS) || defined(PN_TMR)\
	|| defined(PN_INR6_FC)|| defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

void PHY_PowerOnPortCHxSet(void)
{
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)|| defined(PN_FBR8) || defined(PN_FBR4)
    
#if defined (PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_FBR8) 
    // Set PA11 as output (PWM1)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
    // Set PA11 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(11*2))) | (2<<(11*2)); 
    // Set PA11=0 
    GPIOA->BSRR=1UL<<(16+11);
    
    // Set PD0 as output (PWM2)
    GPIOD->MODER=(GPIOD->MODER&~(3UL<<(0*2))) | (1<<(0*2));
    // Set PD0 as Pull-DOWN 
    GPIOD->PUPDR=(GPIOD->PUPDR&~(3UL<<(0*2))) | (2<<(0*2)); 
    // Set PD0=0 
    GPIOD->BSRR=1UL<<(16+0);
    
    // Set PB3 as output (PWM3)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(3*2))) | (1<<(3*2));
    // Set PB3 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(3*2))) | (2<<(3*2)); 
    // Set PB3=0 
    GPIOB->BSRR=1UL<<(16+3);  
    
    // Set PB4 as output (PWM4)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(4*2))) | (1<<(4*2));
    // Set PB4 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(4*2))) | (2<<(4*2)); 
    // Set PB4=0 
    GPIOB->BSRR=1UL<<(16+4); 
 #endif
 
 #if defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B)
    // Set PB5 as output (PWM5)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(5*2))) | (1<<(5*2));
    // Set PB5 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(5*2))) | (2<<(5*2)); 
    // Set PB5=0 
    GPIOB->BSRR=1UL<<(16+5); 
    
    // Set PA8 as output (PWM6)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(8*2))) | (1<<(8*2));
    // Set PA8 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(8*2))) | (2<<(8*2)); 
    // Set PA8=0 
    GPIOA->BSRR=1UL<<(16+8);
    
     // Set PA4 as output (PWM7)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(4*2))) | (1<<(4*2));
    // Set PA4 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(4*2))) | (2<<(4*2)); 
    // Set PA4=0 
    GPIOA->BSRR=1UL<<(16+4);   
    
     // Set PB1 as output (PWM8)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(1*2))) | (1<<(1*2));
    // Set PB1 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(1*2))) | (2<<(1*2)); 
    // Set PB1=0 
    GPIOB->BSRR=1UL<<(16+1); 
#endif

    // Set PB6 as output (PWM9)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(6*2))) | (1<<(6*2));
    // Set PB6 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(6*2))) | (2<<(6*2)); 
    // Set PB6=0 
    GPIOB->BSRR=1UL<<(16+6); 
    
    // Set PB0 as output (PWM10)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (1<<(0*2));
    // Set PB0 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (2<<(0*2)); 
    // Set PB0=0 
    GPIOB->BSRR=1UL<<(16+0); 
    
    // Set PA2 as output (PWM11)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
    // Set PA2 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2<<(2*2)); 
    // Set PA2=0 
    GPIOA->BSRR=1UL<<(16+2);   
    
    // Set PA15 as output (PWM12)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (1<<(15*2));
    // Set PA15 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(15*2))) | (2UL<<(15*2)); 
    // Set PA15=0 
    GPIOA->BSRR=1UL<<(16+15);  

//    // Set PA0 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (1<<(0*2));
//    // Set PA0 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(0*2))) | (2UL<<(0*2)); 
//    // Set PA0=0 
//    GPIOA->BSRR=1UL<<(16+0); 

//    // Set PA1 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(1*2))) | (1<<(1*2));
//    // Set PA1 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (2<<(1*2)); 
//    // Set PA1=0 
//    GPIOA->BSRR=1UL<<(16+1); 

//    // Set PA2 as output (Port B)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
//    // Set PA2 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2UL<<(2*2)); 
//    // Set PA2=0 
//    GPIOA->BSRR=1UL<<(16+2); 

//    // Set PA3 as output (Port B)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(3*2))) | (1<<(3*2));
//    // Set PA3 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(3*2))) | (2<<(3*2)); 
//    // Set PA3=0 
//    GPIOA->BSRR=1UL<<(16+3); 
//    
//     // Set PA5 as output (Port C)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(5*2))) | (1<<(5*2));
//    // Set PA5 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(5*2))) | (2UL<<(5*2)); 
//    // Set PA5=0 
//    GPIOA->BSRR=1UL<<(16+5); 

//    // Set PB0 as output (Port C)
//    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (1<<(0*2));
//    // Set PB0 as Pull-DOWN 
//    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (2<<(0*2)); 
//    // Set PB0=0 
//    GPIOB->BSRR=1UL<<(16+0);  
// 
//    // Set PB6 as output (Port D)
//    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(6*2))) | (1<<(6*2));
//    // Set PB6 as Pull-DOWN 
//    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(6*2))) | (2<<(6*2)); 
//    // Set PB6=0 
//    GPIOB->BSRR=1UL<<(16+6);

//    // Set PB7 as output (Port D)
//    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(7*2))) | (1<<(7*2));
//    // Set PB7 as Pull-DOWN 
//    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(7*2))) | (2<<(7*2)); 
//    // Set PB7=0 
//    GPIOB->BSRR=1UL<<(16+7);
    
#elif defined(PN_INR6_HS) || defined(PN_INR6_FC)

    // Set PB5 as output (PWM1)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(5*2))) | (1<<(5*2));
    // Set PB5 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(5*2))) | (2<<(5*2)); 
    // Set PB5=0 
    GPIOB->BSRR=1UL<<(16+5);

    // Set PA11 as output (PWM2)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
    // Set PA11 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(11*2))) | (2<<(11*2)); 
    // Set PA11=0 
    GPIOA->BSRR=1UL<<(16+11);

    // Set PA1 as output (PWM3)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(1*2))) | (1<<(1*2));
    // Set PA1 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (2<<(1*2)); 
    // Set PA1=0 
    GPIOA->BSRR=1UL<<(16+1);
    
    // Set PB0 as output (PWM4)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(5*2))) | (1<<(5*2));
    // Set PB5 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(5*2))) | (2<<(5*2)); 
    // Set PB5=0 
    GPIOB->BSRR=1UL<<(16+5); 

    // Set PA2 as output (PWM5)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
    // Set PA2 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2<<(2*2)); 
    // Set PA2=0 
    GPIOA->BSRR=1UL<<(16+2);
    
    // Set PA10 as output (PWM6)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (10<<(1*2));
    // Set PA10 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (2<<(10*2)); 
    // Set PA10=0 
    GPIOA->BSRR=1UL<<(16+10);
    
//    // Set PA9 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (1<<(9*2));
//    // Set PA9 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(9*2))) | (2UL<<(9*2)); 
//    // Set PA9=0 
//    GPIOA->BSRR=1UL<<(16+9); 

//    // Set PA10 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (1<<(10*2));
//    // Set PA10 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (2<<(10*2)); 
//    // Set PA10=0 
//    GPIOA->BSRR=1UL<<(16+10); 

//    // Set PA2 as output (Port B)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
//    // Set PA2 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2UL<<(2*2)); 
//    // Set PA2=0 
//    GPIOA->BSRR=1UL<<(16+2); 

//    // Set PA15 as output (Port B)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (1<<(15*2));
//    // Set PA15 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(15*2))) | (2UL<<(15*2)); 
//    // Set PA15=0 
//    GPIOA->BSRR=1UL<<(16+15); 
//    
//    // Set PB0 as output (Port C)
//    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (1<<(0*2));
//    // Set PB0 as Pull-DOWN 
//    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (2<<(0*2)); 
//    // Set PB0=0 
//    GPIOB->BSRR=1UL<<(16+0);  
//    
//    // Set PB2 as output (Port C)
//    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(2*2))) | (1<<(2*2));
//    // Set PB2 as Pull-DOWN 
//    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(2*2))) | (2<<(2*2)); 
//    // Set PB2=0 
//    GPIOB->BSRR=1UL<<(16+2);
// 
//    // Set PA0 as output (Port D)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (1<<(0*2));
//    // Set PA0 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(0*2))) | (2UL<<(0*2)); 
//    // Set PA0=0 
//    GPIOA->BSRR=1UL<<(16+0); 

//    // Set PA1 as output (Port D)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(1*2))) | (1<<(1*2));
//    // Set PA1 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (2<<(1*2)); 
//    // Set PA1=0 
//    GPIOA->BSRR=1UL<<(16+1); 

#elif defined(PN_FGR4D) 
    // Set PA1 as output (PWM1)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(1*2))) | (1<<(1*2));
    // Set PA1 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (2<<(1*2)); 
    // Set PA1=0 
    GPIOA->BSRR=1UL<<(16+1);
    
    // Set PB0 as output (PWM2)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (1<<(0*2));
    // Set PB5 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (2<<(0*2)); 
    // Set PB5=0 
    GPIOB->BSRR=1UL<<(16+5); 

    // Set PA2 as output (PWM3)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
    // Set PA2 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2<<(2*2)); 
    // Set PA2=0 
    GPIOA->BSRR=1UL<<(16+2);
    
    // Set PA10 as output (PWM4)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (1<<(10*2));
    // Set PA10 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (2<<(10*2)); 
    // Set PA10=0 
    GPIOA->BSRR=1UL<<(16+10);
#elif defined(PN_TR8B)
    
    // Set PB5 as output (PWM1)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(5*2))) | (1<<(5*2));
    // Set PB5 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(5*2))) | (2<<(5*2)); 
    // Set PB5=0 
    GPIOB->BSRR=1UL<<(16+5);

    // Set PB4 as output (PWM2)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(4*2))) | (1<<(4*2));
    // Set PB4 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(4*2))) | (2<<(4*2)); 
    // Set PB4=0 
    GPIOB->BSRR=1UL<<(16+4);

    // Set PB3 as output (PWM3)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(3*2))) | (1<<(3*2));
    // Set PB3 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(3*2))) | (2<<(3*2)); 
    // Set PB3=0 
    GPIOB->BSRR=1UL<<(16+3);
    
    // Set PA11 as output (PWM4)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
    // Set PA11 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(11*2))) | (2<<(11*2)); 
    // Set PA11=0 
    GPIOA->BSRR=1UL<<(16+11); 

    // Set PA2 as output (PWM5)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
    // Set PA2 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2<<(2*2)); 
    // Set PA2=0 
    GPIOA->BSRR=1UL<<(16+2);
    
    // Set PB0 as output (PWM6)
    GPIOB->MODER=(GPIOB->MODER&~(3UL<<(0*2))) | (10<<(0*2));
    // Set PB0 as Pull-DOWN 
    GPIOB->PUPDR=(GPIOB->PUPDR&~(3UL<<(0*2))) | (2<<(0*2)); 
    // Set PB0=0 
    GPIOB->BSRR=1UL<<(16+0);

//    // Set PA9 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (1<<(9*2));
//    // Set PA9 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(9*2))) | (2UL<<(9*2)); 
//    // Set PA9=0 
//    GPIOA->BSRR=1UL<<(16+9); 

//    // Set PA10 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (1<<(10*2));
//    // Set PA10 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (2<<(10*2)); 
//    // Set PA10=0 
//    GPIOA->BSRR=1UL<<(16+10); 

//    // Set PA0 as output (Port B) 
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(0*2))) | (1<<(0*2));
//    // Set PA0 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(0*2))) | (2UL<<(0*2)); 
//    // Set PA0=0 
//    GPIOA->BSRR=1UL<<(16+0); 

//    // Set PA1 as output (Port B)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(1*2))) | (1<<(1*2));
//    // Set PA1 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(1*2))) | (2<<(1*2)); 
//    // Set PA1=0 
//    GPIOA->BSRR=1UL<<(16+1);  
#elif defined(PN_TMR)

    // Set PA10 as output(Port A)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (10<<(1*2));
    // Set PA10 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (2<<(10*2)); 
    // Set PA10=0 
    GPIOA->BSRR=1UL<<(16+10);
    
//    // Set PA9 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (1<<(9*2));
//    // Set PA9 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(9*2))) | (2UL<<(9*2)); 
//    // Set PA9=0 
//    GPIOA->BSRR=1UL<<(16+9); 

#elif defined(PN_GMR)

    // Set PA8 as output (PWM1)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(8*2))) | (1<<(8*2));
    // Set PA8 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(8*2))) | (2<<(8*2)); 
    // Set PA8=0 
    GPIOA->BSRR=1UL<<(16+8); 
    
    // Set PA11 as output (PWM2)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(11*2))) | (1<<(11*2));
    // Set PA11 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(11*2))) | (2<<(11*2)); 
    // Set PA11=0 
    GPIOA->BSRR=1UL<<(16+11); 
    
    // Set PA2 as output (PWM3)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(2*2))) | (1<<(2*2));
    // Set PA2 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(2*2))) | (2<<(2*2)); 
    // Set PA2=0 
    GPIOA->BSRR=1UL<<(16+2); 
    
     // Set PA9 as output (PWM4)
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (1<<(9*2));
    // Set PA9 as Pull-DOWN 
    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(9*2))) | (2<<(9*2)); 
    // Set PA9=0 
    GPIOA->BSRR=1UL<<(16+9); 

//    // Set PA3 as output (Port A)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(3*2))) | (1<<(3*2));
//    // Set PA3 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(3*2))) | (2UL<<(3*2)); 
//    // Set PA3=0 
//    GPIOA->BSRR=1UL<<(16+3); 

//    // Set PA10 as output (Port B)
//    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (1<<(10*2));
//    // Set PA10 as Pull-DOWN 
//    GPIOA->PUPDR=(GPIOA->PUPDR&~(3UL<<(10*2))) | (2<<(10*2)); 
//    // Set PA10=0 
//    GPIOA->BSRR=1UL<<(16+10); 

#endif
}


