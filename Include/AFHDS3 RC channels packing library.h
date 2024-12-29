#if !defined(AFHDS3_RC_CHANNELS_PACKING_LIBRARY_H)
#define AFHDS3_RC_CHANNELS_PACKING_LIBRARY_H

#include "System definitions.h"

unsigned long SES_PackChannels(const signed short *pChannels,const unsigned char *pChannelsType,void *pPackedChannels);
void SES_PackOneChannel(unsigned long ChannelNb,signed long ChannelValue,const unsigned char *pChannelsType,void *pPackedChannels);
void SES_UnpackChannels(const void *pPackedChannels,signed short *pChannels,const unsigned char *pChannelsType);

#endif // !defined(AFHDS3_RC_CHANNELS_PACKING_LIBRARY_H)
