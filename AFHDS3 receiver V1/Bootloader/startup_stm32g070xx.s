;******************************************************************************
;* File Name          : startup_stm32g070xx.s
;* Author             : MCD Application Team
;* Description        : STM32G070xx devices vector table for MDK-ARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler
;*                      - Set the vector table entries with the exceptions ISR address
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the CortexM0 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>
;****************************************************************************** 
;* @attention
;*
;* Copyright (c) 2018 STMicroelectronics. All rights reserved.
;*
;* This software component is licensed by ST under BSD 3-Clause license,
;* the "License"; You may not use this file except in compliance with the 
;* License. You may obtain a copy of the License at:
;*                        opensource.org/licenses/BSD-3-Clause
;*
;******************************************************************************

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size		EQU     0x400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size      EQU     0x000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                   ; Top of Stack
                DCD     Reset_Handler                  ; Reset Handler
                DCD     NMI_Handler                    ; NMI Handler
                DCD     HardFault_Handler              ; Hard Fault Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     SVC_Handler                    ; SVCall Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     PendSV_Handler                 ; PendSV Handler
                DCD     SysTick_Handler                ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler                ; Window Watchdog
                DCD     0                              ; Reserved
                DCD     RTC_TAMP_IRQHandler            ; RTC through EXTI Line
                DCD     FLASH_IRQHandler               ; FLASH
                DCD     RCC_IRQHandler                 ; RCC
                DCD     EXTI0_1_IRQHandler             ; EXTI Line 0 and 1
                DCD     EXTI2_3_IRQHandler             ; EXTI Line 2 and 3
                DCD     EXTI4_15_IRQHandler            ; EXTI Line 4 to 15
                DCD     0                              ; Reserved
                DCD     DMA1_Channel1_IRQHandler       ; DMA1 Channel 1
                DCD     DMA1_Channel2_3_IRQHandler     ; DMA1 Channel 2 and Channel 3
                DCD     DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler ; DMA1 Channel 4 to Channel 7, DMAMUX1 overrun
                DCD     ADC1_IRQHandler                ; ADC1
                DCD     TIM1_BRK_UP_TRG_COM_IRQHandler ; TIM1 Break, Update, Trigger and Commutation
                DCD     TIM1_CC_IRQHandler             ; TIM1 Capture Compare
                DCD     0                              ; Reserved
                DCD     TIM3_IRQHandler                ; TIM3
                DCD     TIM6_IRQHandler                ; TIM6
                DCD     TIM7_IRQHandler                ; TIM7
                DCD     TIM14_IRQHandler               ; TIM14
                DCD     TIM15_IRQHandler               ; TIM15
                DCD     TIM16_IRQHandler               ; TIM16
                DCD     TIM17_IRQHandler               ; TIM17
                DCD     I2C1_IRQHandler                ; I2C1
                DCD     I2C2_IRQHandler                ; I2C2
                DCD     SPI1_IRQHandler                ; SPI1
                DCD     SPI2_IRQHandler                ; SPI2
                DCD     USART1_IRQHandler              ; USART1
                DCD     USART2_IRQHandler              ; USART2
                DCD     USART3_4_IRQHandler            ; USART3, USART4

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler routine
Reset_Handler    PROC
                 EXPORT  Reset_Handler                 [WEAK]
        IMPORT  __main
        IMPORT  SystemInit  
                 LDR     R0, =SystemInit
                 BLX     R0
                 LDR     R0, =__main
                 BX      R0
                 ENDP

DispatchInterrupt
				ldr     r2,=0x2000007C ; RAM_IsFirmwareRunning
				ldr     r2,[r2]
				cmp     r2,#0
				bne     DispatchInterrupt2
				bx      r0
DispatchInterrupt2
				ldr     r1,[r1]
				bx      r1

				MACRO
				DispatchedInterrupt $BootloaderAddress,$FirmwareVector
				ldr     r0,=$BootloaderAddress
				ldr     r1,=$FirmwareVector
				b       DispatchInterrupt
                EXPORT  $BootloaderAddress      [WEAK]
				MEND


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
				DispatchedInterrupt NMI_Handler2,0x08006008
NMI_Handler2
                B       .
                ENDP

HardFault_Handler\
                PROC
				DispatchedInterrupt HardFault_Handler2,0x0800600C
HardFault_Handler2
                B       .
                ENDP
					
SVC_Handler     PROC
				DispatchedInterrupt SVC_Handler2,0x0800602C
SVC_Handler2
				B       .
                ENDP

PendSV_Handler  PROC
				DispatchedInterrupt PendSV_Handler2,0x08006038
PendSV_Handler2
                B       .
                ENDP

SysTick_Handler PROC
				DispatchedInterrupt SysTick_Handler2,0x0800603C
SysTick_Handler2
                B       .
                ENDP

WWDG_IRQHandler PROC
				DispatchedInterrupt WWDG_IRQHandler2,0x08006040
                ENDP
RTC_TAMP_IRQHandler PROC
				DispatchedInterrupt RTC_TAMP_IRQHandler2,0x08006048
                ENDP
FLASH_IRQHandler PROC
				DispatchedInterrupt FLASH_IRQHandler2,0x0800604C
                ENDP
RCC_IRQHandler PROC
				DispatchedInterrupt RCC_IRQHandler2,0x08006050
                ENDP
EXTI0_1_IRQHandler PROC
				DispatchedInterrupt EXTI0_1_IRQHandler2,0x08006054
                ENDP
EXTI2_3_IRQHandler PROC
				DispatchedInterrupt EXTI2_3_IRQHandler2,0x08006058
                ENDP
EXTI4_15_IRQHandler PROC
				DispatchedInterrupt EXTI4_15_IRQHandler2,0x0800605C
                ENDP
DMA1_Channel1_IRQHandler PROC
				DispatchedInterrupt DMA1_Channel1_IRQHandler2,0x08006064
                ENDP
DMA1_Channel2_3_IRQHandler PROC
				DispatchedInterrupt DMA1_Channel2_3_IRQHandler2,0x08006068
                ENDP
DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler PROC
				DispatchedInterrupt DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler2,0x0800606C
                ENDP
ADC1_IRQHandler PROC
				DispatchedInterrupt ADC1_IRQHandler2,0x08006070
                ENDP
TIM1_BRK_UP_TRG_COM_IRQHandler PROC
				DispatchedInterrupt TIM1_BRK_UP_TRG_COM_IRQHandler2,0x08006074
                ENDP
TIM1_CC_IRQHandler PROC
				DispatchedInterrupt TIM1_CC_IRQHandler2,0x08006078
                ENDP
TIM3_IRQHandler PROC
				DispatchedInterrupt TIM3_IRQHandler2,0x08006080
                ENDP
TIM6_IRQHandler PROC
				DispatchedInterrupt TIM6_IRQHandler2,0x08006084
                ENDP
TIM7_IRQHandler PROC
				DispatchedInterrupt TIM7_IRQHandler2,0x08006088
                ENDP				
TIM14_IRQHandler PROC
				DispatchedInterrupt TIM14_IRQHandler2,0x0800608C
                ENDP
TIM15_IRQHandler PROC
				DispatchedInterrupt TIM15_IRQHandler2,0x08006090
                ENDP
TIM16_IRQHandler PROC
				DispatchedInterrupt TIM16_IRQHandler2,0x08006094
                ENDP
TIM17_IRQHandler PROC
				DispatchedInterrupt TIM17_IRQHandler2,0x08006098
                ENDP
I2C1_IRQHandler PROC
				DispatchedInterrupt I2C1_IRQHandler2,0x0800609C
                ENDP
I2C2_IRQHandler PROC
				DispatchedInterrupt I2C2_IRQHandler2,0x080060A0
                ENDP
SPI1_IRQHandler PROC
				DispatchedInterrupt SPI1_IRQHandler2,0x080060A4
                ENDP
SPI2_IRQHandler PROC
				DispatchedInterrupt SPI2_IRQHandler2,0x080060A8
                ENDP
USART1_IRQHandler PROC
				DispatchedInterrupt USART1_IRQHandler2,0x080060AC
                ENDP
USART2_IRQHandler PROC
				DispatchedInterrupt USART2_IRQHandler2,0x080060B0
                ENDP
USART3_4_IRQHandler PROC
				DispatchedInterrupt USART3_4_IRQHandler2,0x080060B4
                ENDP		
		

Default_Handler PROC

                EXPORT  WWDG_IRQHandler2                [WEAK]
                EXPORT  RTC_TAMP_IRQHandler2            [WEAK]
                EXPORT  FLASH_IRQHandler2               [WEAK]
                EXPORT  RCC_IRQHandler2                 [WEAK]
                EXPORT  EXTI0_1_IRQHandler2             [WEAK]
                EXPORT  EXTI2_3_IRQHandler2             [WEAK]
                EXPORT  EXTI4_15_IRQHandler2            [WEAK]
                EXPORT  DMA1_Channel1_IRQHandler2       [WEAK]
                EXPORT  DMA1_Channel2_3_IRQHandler2     [WEAK]
                EXPORT  DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler2 [WEAK]
                EXPORT  ADC1_IRQHandler2                [WEAK]
                EXPORT  TIM1_BRK_UP_TRG_COM_IRQHandler2 [WEAK]
                EXPORT  TIM1_CC_IRQHandler2             [WEAK]
                EXPORT  TIM3_IRQHandler2                [WEAK]
                EXPORT  TIM6_IRQHandler2                [WEAK]
                EXPORT  TIM7_IRQHandler2                [WEAK]
                EXPORT  TIM14_IRQHandler2               [WEAK]
                EXPORT  TIM15_IRQHandler2               [WEAK]
                EXPORT  TIM16_IRQHandler2               [WEAK]
                EXPORT  TIM17_IRQHandler2               [WEAK]
                EXPORT  I2C1_IRQHandler2                [WEAK]
                EXPORT  I2C2_IRQHandler2                [WEAK]
                EXPORT  SPI1_IRQHandler2                [WEAK]
                EXPORT  SPI2_IRQHandler2                [WEAK]
                EXPORT  USART1_IRQHandler2              [WEAK]
                EXPORT  USART2_IRQHandler2              [WEAK]
                EXPORT  USART3_4_IRQHandler2            [WEAK]


WWDG_IRQHandler2
RTC_TAMP_IRQHandler2
FLASH_IRQHandler2
RCC_IRQHandler2
EXTI0_1_IRQHandler2
EXTI2_3_IRQHandler2
EXTI4_15_IRQHandler2
DMA1_Channel1_IRQHandler2
DMA1_Channel2_3_IRQHandler2
DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler2
ADC1_IRQHandler2
TIM1_BRK_UP_TRG_COM_IRQHandler2
TIM1_CC_IRQHandler2
TIM3_IRQHandler2
TIM6_IRQHandler2
TIM7_IRQHandler2
TIM14_IRQHandler2
TIM15_IRQHandler2
TIM16_IRQHandler2
TIM17_IRQHandler2
I2C1_IRQHandler2
I2C2_IRQHandler2
SPI1_IRQHandler2
SPI2_IRQHandler2
USART1_IRQHandler2
USART2_IRQHandler2
USART3_4_IRQHandler2

                B       .

                ENDP

	EXPORT LaunchFirmware
LaunchFirmware
	LDR   R0,=0x08006000
	LDR   R0,[R0]
	MOV   SP,R0
	LDR   R0,=0x08006004
	LDR   R0,[R0]
	BX    R0

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE*****
