#if !defined(USART_H)
#define USART_H

#define USART_PARITY_NONE 0
#define USART_PARITY_ODD  1
#define USART_PARITY_EVEN 2
#define USART_STOPBIT_1   0
#define USART_STOPBIT_1_5 1
#define USART_STOPBIT_2   2
#define USART_BAUDRATE_TO_BIT_DURATION(BaudRate) ((500000000+BaudRate/2)/BaudRate)
typedef struct __attribute__((packed))
{
	unsigned short BitDuration; // Duration of one bit on the USART in units of 2ns
	unsigned char Parity:2;
	unsigned char StopBits:2;
	unsigned char Inverted:1;
	unsigned char Reserved:3;
} sUSART_Config;

#endif // !defined(USART_H)
