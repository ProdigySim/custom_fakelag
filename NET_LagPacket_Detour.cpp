#include "extension.h"
#include "NET_LagPacket_Detour.h"
#include "PlayerLagManager.h"
#include "LagSystem.h"

IForward* g_fwdLagPacket = NULL;
CDetour* DLagPacket = NULL;

static const PlayerLagManager* s_LagManager;
static LagSystem* s_LagSystem;

float getLagPacketMs(const _netpacket_t & packet)
{
	return s_LagManager->GetPlayerLag(packet.from);
}

DETOUR_DECL_STATIC2(NET_LagPacket, bool, bool, newdata, _netpacket_t*, packet)
{

	// If "newdata" is true, packet points to a new packet of real data (which we could potentially lag)
	// If "newdata" is false, packet points to an empty, but initialized packet struct.
	// Returning true means we assert that "packet" is full of a consumable network packet.
	// Returning false means the consumer should ignore packet, there is no packet to consume yet.
	if (newdata) {
		const float lagTime = getLagPacketMs(*packet);
		if (lagTime > 0.0) {
			// g_pSM->LogError(myself, "Lagging packet on socket %d for %fms", packet->source, lagTime);
			s_LagSystem->LagPacket(packet, lagTime);
		}
		else
		{
			// TODO: Flush lagged packets if the lag time is reduced or removed?
			return true;
		}
	}

	return s_LagSystem->GetNextPacket(packet->source, packet);
}

bool CreateNetLagPacketDetour()
{
	DLagPacket = DETOUR_CREATE_STATIC(NET_LagPacket, "NET_LagPacket");
	if (DLagPacket == NULL)
	{
		g_pSM->LogError(myself, "NET_LagPacket detour could not be initialized - FeelsBadMan.");
		return false;
	}
	DLagPacket->EnableDetour();
	return true;
}

void RemoveNetLagPacketDetour()
{
	if (DLagPacket != NULL)
	{
		DLagPacket->Destroy();
		DLagPacket = NULL;
	}
}


// lagManager: A Lag Manager instance to look up player lag times
// pNetTime: Pointer to the engine "net_time" variable
bool LagDetour_Init(const PlayerLagManager* lagManager, const double* pNetTime)
{
	s_LagManager = lagManager;
	s_LagSystem = new LagSystem(pNetTime);
	bool detoured = CreateNetLagPacketDetour();
	if (!detoured) {
		LagDetour_Shutdown();
	}
	return detoured;
}

void LagDetour_Shutdown() {
	RemoveNetLagPacketDetour();
	if (s_LagSystem != NULL)
	{
		delete s_LagSystem;
	}
	s_LagManager = NULL;
}