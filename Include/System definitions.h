#if !defined(SYSTEM_DEFINITIONS_H)
#define SYSTEM_DEFINITIONS_H

#define BOOL  unsigned char
#define TRUE  1UL
#define FALSE 0UL
#define NULL  0

#define MIN(X,Y) (((X)<(Y))?(X):(Y))
#define MAX(X,Y) (((X)>(Y))?(X):(Y))

#if defined(__ARM_FEATURE_IDIV)
	#define DIV10(N) (N/10)
#else
	#define DIV10(N) ((((unsigned long)N)*52429)>>19) // Valid from 0 to 81919
#endif

#if defined(__ARM_FEATURE_UNALIGNED)
	#define SYS_LoadULong(pPointer) (*(const unsigned long)(pPointer))
	#define SYS_LoadUShort(pPointer) (*(const unsigned short)(pPointer))
	#define SYS_StoreULong(pPointer,Value) *(unsigned long *)(pPointer)=Value
	#define SYS_StoreUShort(pPointer,Value) *(unsigned short *)(pPointer)=Value
#else
	#define SYS_LoadULong(pPointer) ((unsigned long) \
									 (((*((const unsigned char *)(pPointer)+0))<< 0) | \
									  ((*((const unsigned char *)(pPointer)+1))<< 8) | \
									  ((*((const unsigned char *)(pPointer)+2))<<16) | \
									  ((*((const unsigned char *)(pPointer)+3))<<24)))
	#define SYS_LoadUShort(pPointer) ((unsigned short) \
									  (((*((const unsigned char *)(pPointer)+0))<< 0) | \
									   ((*((const unsigned char *)(pPointer)+1))<< 8)))
	#define SYS_STORE_ULONG(pPointer,Value) {*((unsigned char *)(pPointer)+0)=(unsigned char)((Value)>> 0); \
											 *((unsigned char *)(pPointer)+1)=(unsigned char)((Value)>> 8); \
											 *((unsigned char *)(pPointer)+2)=(unsigned char)((Value)>>16); \
											 *((unsigned char *)(pPointer)+3)=(unsigned char)((Value)>>24);}
	#define SYS_STORE_USHORT(pPointer,Value) {*((unsigned char *)(pPointer)+0)=(unsigned char)((Value)>> 0); \
											  *((unsigned char *)(pPointer)+1)=(unsigned char)((Value)>> 8);}
#endif
							   
#endif // !defined(SYSTEM_DEFINITIONS_H)
