#if !defined(ENCRYPTION_H)
#define ENCRYPTION_H


// UID_BASE = UID_BASE1 + UID_BASE2
#define UID_BASE1              (0x5476253UL)         
#define UID_BASE2              (UID_BASE-UID_BASE1)  //(0x1AB8133DUL)      


void Encryption_Check(void);


#endif // !defined(ENCRYPTION_H)

