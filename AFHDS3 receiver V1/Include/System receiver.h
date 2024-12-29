#if !defined(SYSTEM_RECEIVER)
#define SYSTEM_RECEIVER

#include "System definitions.h"

#define FLASH_BASE_ADDRESS    0x08000000
#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
	|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B) || defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
	#define SYS_FLASH_SIZE            0x20000
	#define BOOTLOADER_SIZE           0x6000
	#define FLASH_PAGE_SIZE           0x800
#elif defined(PN_GMR)
	#define SYS_FLASH_SIZE            0x10000
	#define BOOTLOADER_SIZE           0x3000
	#define FLASH_PAGE_SIZE           0x400
#endif

#define MAX_FIRMWARE_SIZE     (SYS_FLASH_SIZE-BOOTLOADER_SIZE-4)
#define FIRMWARE_BASE_ADDRESS (FLASH_BASE_ADDRESS+BOOTLOADER_SIZE)

#if defined(BOOTLOADER)
	extern const unsigned long  BOOT_ProductNumber;
	extern const unsigned short BOOT_HardwareVersion;
	extern const unsigned short BOOT_MainboardVersion;
	extern const unsigned short BOOT_RFModuleVersion;
	extern const unsigned short BOOT_BootloaderVersion;
#else
	#define BOOT_ProductNumber     (*(unsigned long  *)0x08000200)
	#define BOOT_HardwareVersion   (*(unsigned short *)0x08000204)
	#define BOOT_MainboardVersion  (*(unsigned short *)0x08000208)
	#define BOOT_RFModuleVersion   (*(unsigned short *)0x0800020C)
	#define BOOT_BootloaderVersion (*(unsigned short *)0x08000210)
#endif

#if defined(FLASH_RXID)
	extern const unsigned long  CONFIG_RXID;
	extern const unsigned short CONFIG_CompanyCode;
#else
	#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12)  || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
		|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		#define EncryptionCode1	    (*(const unsigned long  *)0x08005000)
		#define EncryptionCode2	    (*(const unsigned long  *)0x08005004)
		#define EncryptionCode3	    (*(const unsigned long  *)0x08005008)
		#define CONFIG_RXID         (*(const unsigned long  *)0x08005800)
		#define CONFIG_CompanyCode  (*(const unsigned short *)0x08005804)
	#elif defined(PN_GMR)
		#define EncryptionCode1	   (*(const unsigned long  *)0x08002800)
		#define EncryptionCode2	   (*(const unsigned long  *)0x08002804)
		#define EncryptionCode3	   (*(const unsigned long  *)0x08002808)
		#define CONFIG_RXID        (*(const unsigned long  *)0x08002C00)
		#define CONFIG_CompanyCode (*(const unsigned short *)0x08002C04)
	#endif
#endif

#define FIRMWARE_SIGNATURE_1 0xC87251CC
#define FIRMWARE_SIGNATURE_2 0x92017EA0
#define FIRMWARE_DEBUG_SIGNATURE_1 0x12345678
#define FIRMWARE_DEBUG_SIGNATURE_2 0x87654321
#if defined(BOOTLOADER)
	#if defined(PN_FGR4B) || defined(PN_FTR8B) || defined(PN_FGR8B) || defined(PN_FTR12B) || defined(PN_FBR12) || defined(PN_FGR12B) || defined(PN_INR4_GYB)\
		|| defined(PN_INR6_HS) || defined(PN_TMR) || defined(PN_INR6_FC) || defined(PN_TR8B)|| defined(PN_FBR8) || defined(PN_FBR4) || defined(PN_FGR4D)
		#define FIRMWARE_Signature1 (*(unsigned long  *)0x08006F80)
		#define FIRMWARE_Signature2 (*(unsigned long  *)0x08006F84)
		#define FIRMWARE_Length     (*(unsigned long  *)0x08006F88)
		#define FIRMWARE_Version    (*(unsigned short *)0x08006F8C)
	#elif defined(PN_GMR)
		#define FIRMWARE_Signature1 (*(unsigned long  *)0x08003F80)
		#define FIRMWARE_Signature2 (*(unsigned long  *)0x08003F84)
		#define FIRMWARE_Length     (*(unsigned long  *)0x08003F88)
		#define FIRMWARE_Version    (*(unsigned short *)0x08003F8C)
	#endif
#else
	extern const unsigned long  FIRMWARE_Signature1;
	extern const unsigned long  FIRMWARE_Signature2;
	extern const unsigned short FIRMWARE_Version;
	extern const unsigned long  FIRMWARE_Length;
#endif

#define RESET_SIGNATURE_1 0x1AE811C9
#define RESET_SIGNATURE_2 0x7F5594AC
#define RAM_ResetSignature1   (*(unsigned long  *)0x20000000)
#define RAM_ResetSignature2   (*(unsigned long  *)0x20000004)
#define RAM_ResetMode         (*(unsigned long  *)0x20000008)
#define RAM_IsBinding         (*(unsigned long  *)0x2000000C)
#define RAM_BindSignature1    (*(unsigned long  *)0x20000074) // Used by the firmware only
#define RAM_BindSignature2    (*(unsigned long  *)0x20000078) // Used by the firmware only
#define RAM_IsFirmwareRunning (*(unsigned long  *)0x2000007C)

#endif // !defined(SYSTEM_RECEIVER)
