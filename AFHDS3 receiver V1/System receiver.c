#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) \
	|| defined(PN_INR6_HS) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#include <stm32g0xx.h>
#elif defined(PN_GMR)
	#include <stm32f0xx.h>
#endif

#include "System definitions.h"
#include "System.h"

#include "Update.h"

#include "System receiver.h"

#pragma clang diagnostic ignored "-Wunused-const-variable"

#if defined(BOOTLOADER)
	#if defined(PN_FGR4B)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FGR4B;
	#elif defined(PN_FTR8B)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FTR8B;
	#elif defined(PN_FTR12B)  
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FTR12B;
	#elif defined(PN_FGR8B)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FGR8B;
	#elif defined(PN_FGR12B)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FGR12B;
	#elif defined(PN_INR4_GYB)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_INR4_GYB;
	#elif defined(PN_INR6_HS)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_INR6_HS;
	#elif defined(PN_INR6_FC)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_INR6_FC;
	#elif defined(PN_GMR)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_GMR;
	#elif defined(PN_TMR)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_TMR;
	#elif defined(PN_FBR12)		
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FBR12;
	#elif defined(PN_FBR8)		
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FBR8;
	#elif defined(PN_FBR4)		
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FBR4;        
	#elif defined(PN_TR8B)
		const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_TR8B;
	#elif defined(PN_FGR4D)
	    const unsigned long BOOT_ProductNumber                  __attribute__((section(".ARM.__at_0x08000200"),used))=UPD_PN_FGR4D;
	#endif
	const unsigned short BOOT_HardwareVersion                   __attribute__((section(".ARM.__at_0x08000204"),used))=0x0100;
	
	#if defined(PN_FGR4B)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0100;
	#elif defined(PN_FTR8B)
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0102;
	#elif defined(PN_FGR8B)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0102;
	#elif defined(PN_FGR12B)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0103;
	#elif defined(PN_FTR12B) 	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;
	#elif defined(PN_FBR12)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;
	#elif defined(PN_FBR8)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;
	#elif defined(PN_FBR4)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;        
	#elif defined(PN_GMR)
        const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0100;
    #elif defined(PN_TMR)	
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0100;
	#elif defined(PN_INR4_GYB)
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;  
	#elif defined(PN_INR6_HS)
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0105;	
	#elif defined(PN_INR6_FC)
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0100;	
	#elif defined(PN_TR8B)
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;
    #elif defined(PN_FGR4D)
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;		
	#else
		const unsigned short BOOT_MainboardVersion                  __attribute__((section(".ARM.__at_0x08000208"),used))=0x0101;
	#endif	
	
	const unsigned short BOOT_RFModuleVersion                   __attribute__((section(".ARM.__at_0x0800020C"),used))=0x0100;
	const unsigned short BOOT_BootloaderVersion                 __attribute__((section(".ARM.__at_0x08000210"),used))=0x0201;
	static const unsigned long  BOOT_BootloaderReserved[0x6C/4] __attribute__((section(".ARM.__at_0x08000214"),used))={0};
#else
	#define BOOT_ProductNumber     (*(unsigned long  *)0x08000200)
	#define BOOT_HardwareVersion   (*(unsigned short *)0x08000204)
	#define BOOT_MainboardVersion  (*(unsigned short *)0x08000208)
	#define BOOT_RFModuleVersion   (*(unsigned short *)0x0800020C)
	#define BOOT_BootloaderVersion (*(unsigned short *)0x08000210)
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) \
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#if defined(FLASH_RXID)
		const unsigned long  CONFIG_RXID                               __attribute__((section(".ARM.__at_0x08005800"),used))=0x55556768;
		const unsigned short CONFIG_CompanyCode                        __attribute__((section(".ARM.__at_0x08005804"),used))=UPD_CC_FLYSKY;
	#else
		#define EncryptionCode1	   (*(const unsigned long  *)0x08005000)
		#define EncryptionCode2	   (*(const unsigned long  *)0x08005004)
		#define EncryptionCode3	   (*(const unsigned long  *)0x08005008)
		#define CONFIG_RXID        (*(const unsigned long  *)0x08005800)
		#define CONFIG_CompanyCode (*(const unsigned short *)0x08005804)
	#endif
#elif defined(PN_GMR)
	#if defined(FLASH_RXID)
		const unsigned long  CONFIG_RXID                               __attribute__((section(".ARM.__at_0x08002C00"),used))=0x55556668;
		const unsigned short CONFIG_CompanyCode                        __attribute__((section(".ARM.__at_0x08002C04"),used))=UPD_CC_FLYSKY;
	#else
		#define EncryptionCode1	   (*(const unsigned long  *)0x08002800)
		#define EncryptionCode2	   (*(const unsigned long  *)0x08002804)
		#define EncryptionCode3	   (*(const unsigned long  *)0x08002808)
		#define CONFIG_RXID        (*(const unsigned long  *)0x08002C00)
		#define CONFIG_CompanyCode (*(const unsigned short *)0x08002C04)
	#endif
#endif

#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB) \
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC)|| defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#if defined(BOOTLOADER)
		#define FIRMWARE_Signature1 (*(unsigned long  *)0x08006F80)
		#define FIRMWARE_Signature2 (*(unsigned long  *)0x08006F84)
		#define FIRMWARE_Length     (*(unsigned long  *)0x08006F88)
		#define FIRMWARE_Version    (*(unsigned short *)0x08006F8C)
	#else
		#if defined(DEBUG)
			const unsigned long FIRMWARE_Signature1                   __attribute__((section(".ARM.__at_0x08006F80"),used))=FIRMWARE_DEBUG_SIGNATURE_1;
			const unsigned long FIRMWARE_Signature2                   __attribute__((section(".ARM.__at_0x08006F84"),used))=FIRMWARE_DEBUG_SIGNATURE_2;
		#else
			const unsigned long FIRMWARE_Signature1                   __attribute__((section(".ARM.__at_0x08006F80"),used))=FIRMWARE_SIGNATURE_1;
			const unsigned long FIRMWARE_Signature2                   __attribute__((section(".ARM.__at_0x08006F84"),used))=FIRMWARE_SIGNATURE_2;
		#endif
		const unsigned long  FIRMWARE_Length                          __attribute__((section(".ARM.__at_0x08006F88"),used))=0x12345678;
		
		#if defined(PN_FGR4B)
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0103;
		#elif defined(PN_FTR8B)
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0115;
		#elif defined(PN_FGR8B)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0114;
		#elif defined(PN_FGR12B)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x010C;
		#elif defined(PN_FTR12B)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0107;
		#elif defined(PN_FBR12)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0104;
        #elif defined(PN_FBR8)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0101;
		#elif defined(PN_FBR4)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0101;            
		#elif defined(PN_TMR)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0104;
		#elif defined(PN_INR4_GYB)	
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0108;
		#elif defined(PN_INR6_HS)
			const unsigned short FIRMWARE_Version               		   __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0108;	
		#elif defined(PN_INR6_FC)
			const unsigned short FIRMWARE_Version                			  __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0105;	
		#elif defined(PN_TR8B)
			const unsigned short FIRMWARE_Version                			  __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0103;
		#elif defined(PN_FGR4D)
			const unsigned short FIRMWARE_Version                			  __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0101;
		#else
			const unsigned short FIRMWARE_Version                         __attribute__((section(".ARM.__at_0x08006F8C"),used))=0x0105;
		#endif
				
		static const unsigned long  FIRMWARE_Reserved[0x70/4]         __attribute__((section(".ARM.__at_0x08006F90"),used));
	#endif
#elif defined(PN_GMR)
	#if defined(BOOTLOADER)
		#define FIRMWARE_Signature1 (*(unsigned long  *)0x08003F80)
		#define FIRMWARE_Signature2 (*(unsigned long  *)0x08003F84)
		#define FIRMWARE_Length     (*(unsigned long  *)0x08003F88)
		#define FIRMWARE_Version    (*(unsigned short *)0x08003F8C)
	#else
		#if defined(DEBUG)
			const unsigned long FIRMWARE_Signature1            __attribute__((section(".ARM.__at_0x08003F80"),used))=FIRMWARE_DEBUG_SIGNATURE_1;
			const unsigned long FIRMWARE_Signature2            __attribute__((section(".ARM.__at_0x08003F84"),used))=FIRMWARE_DEBUG_SIGNATURE_2;
		#else
			const unsigned long FIRMWARE_Signature1            __attribute__((section(".ARM.__at_0x08003F80"),used))=FIRMWARE_SIGNATURE_1;
			const unsigned long FIRMWARE_Signature2            __attribute__((section(".ARM.__at_0x08003F84"),used))=FIRMWARE_SIGNATURE_2;
		#endif
		const unsigned long  FIRMWARE_Length                   __attribute__((section(".ARM.__at_0x08003F88"),used))=0x12345678;
		const unsigned short FIRMWARE_Version                  __attribute__((section(".ARM.__at_0x08003F8C"),used))=0x010E;
		static const unsigned long  FIRMWARE_Reserved[0x70/4]         __attribute__((section(".ARM.__at_0x08003F90"),used));
	#endif
#endif

#define RAM_ResetSignature1   (*(unsigned long  *)0x20000000)
#define RAM_ResetSignature2   (*(unsigned long  *)0x20000004)
#define RAM_ResetMode         (*(unsigned long  *)0x20000008)
#define RAM_IsBinding         (*(unsigned long  *)0x2000000C)
#define RAM_BindSignature1    (*(unsigned long  *)0x20000074) // Used by the firmware only
#define RAM_BindSignature2    (*(unsigned long  *)0x20000078) // Used by the firmware only
#define RAM_IsFirmwareRunning (*(unsigned long  *)0x2000007C)
    
#pragma clang diagnostic warning "-Wunused-const-variable"
