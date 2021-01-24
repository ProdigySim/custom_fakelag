#pragma once
#include <inetchannel.h>

void AddToLagged(netpacket_t* pPacket, float lagTime);
bool GetNextPacket(int socket, netpacket_t* destPacket);


void SetNetTimePtr(double* net_time);
