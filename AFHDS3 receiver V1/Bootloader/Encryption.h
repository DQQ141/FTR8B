#if !defined(ENCRYPTION_H)
#define ENCRYPTION_H


#if defined (PN_GMR)
	#define ENCRYPTION_BASE_ADDRESS    0x08002400
	#define ENCRYPTION_DATA_ADDRESS    0x08002800
	#define ENCRYPTION_SIGNATURE       0x5846E2C1
#else
	#define ENCRYPTION_BASE_ADDRESS    0x08004800
	#define ENCRYPTION_DATA_ADDRESS    0x08005000
	#define ENCRYPTION_SIGNATURE       0x5846E2C1
#endif

extern const unsigned long EncryptionFlag;

void  Encrypt_WriteCode(void);

#endif // !defined(ENCRYPTION_H)

