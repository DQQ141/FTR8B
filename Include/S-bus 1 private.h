#if !defined(S_BUS_1_H)
#define S_BUS_1_H

#define SB1_USART_SPEED 100000
#define SB1_CYCLE_TIME    7000

#define SB1_NB_CHANNELS            18
#define SB1_NB_ANALOG_CHANNELS     16
#define SB1_NB_ANALOG_CHANNEL_BITS 11

typedef struct __attribute__((packed))
{
	unsigned char Header;
	unsigned char Channels[(SB1_NB_ANALOG_CHANNELS*SB1_NB_ANALOG_CHANNEL_BITS+7)/8];
	unsigned char Flags;
	unsigned char Zero;
} sSB1_Packet;

#endif // !defined(S_BUS_1_H)
