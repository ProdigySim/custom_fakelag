#ifndef _INCLUDE_CUSTOM_FAKELAG_LAGPACKET_DETOUR_H_
#define _INCLUDE_CUSTOM_FAKELAG_LAGPACKET_DETOUR_H_

#include "PlayerLagManager.h"

// lagManager: A Lag Manager instance to look up player lag times
// pNetTime: Pointer to the engine "net_time" variable
bool LagDetour_Init(const PlayerLagManager* lagManager, const double* pNetTime);
void LagDetour_Shutdown();

#endif // _INCLUDE_CUSTOM_FAKELAG_LAGPACKET_DETOUR_H_
