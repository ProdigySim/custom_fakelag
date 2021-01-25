#ifndef _CUSTOM_FAKELAG_NET_LAGPACKET_H_
#define _CUSTOM_FAKELAG_NET_LAGPACKET_H_
#include "net_structures.h"

void AddToLagged(netpacket_t* pPacket, float lagTime);
bool GetNextPacket(int socket, netpacket_t* destPacket);


void SetNetTimePtr(double* net_time);

#endif // _CUSTOM_FAKELAG_NET_LAGPACKET_H_