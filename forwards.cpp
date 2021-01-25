#include "extension.h"
#include "forwards.h"
#include "Net_LagPacket.h"

IForward* g_fwdLagPacket = NULL;
CDetour* DLagPacket = NULL;

float getLagPacketMs(netpacket_t* packet)
{
	float lagTime = 0.0f;
	g_fwdLagPacket->PushFloatByRef(&lagTime);
	g_fwdLagPacket->PushCell(packet->from.ip);
	g_fwdLagPacket->Execute();

	return lagTime;
}

DETOUR_DECL_STATIC2(NET_LagPacket, bool, bool, newdata, netpacket_t *, packet) 
{

	// If "newdata" is true, packet points to a new packet of real data (which we could potentially lag)
	// If "newdata" is false, packet points to an empty, but initialized packet struct.
	// Returning true means we assert that "packet" is full of a consumable network packet.
	// Returning false means the consumer should ignore packet, there is no packet to consume yet.
	if (newdata) {
		float lagTime = getLagPacketMs(packet);
		if (lagTime > 0.0) {
			AddToLagged(packet, lagTime);
		}
		// Do we need to zero out the packet? No right?
	}

	return GetNextPacket(packet->source, packet);
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