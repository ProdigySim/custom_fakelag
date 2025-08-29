#include "extension.h"
#include "NET_LagPacket_Detour.h"
#include "net_ws_queued_packet_sender.h"
#include "PlayerLagManager.h"
#include "LagSystem.h"

// IForward* g_fwdLagPacket = NULL;
CDetour* DLagPacket = NULL;
CDetour* DSendToImpl = NULL;
CDetour* DClearQueuedPacketsForChannel = NULL;

static const PlayerLagManager* s_LagManager = nullptr;
static LagSystem* s_LagSystem = nullptr;

float getLagPacketMs(const dumb_netadr_s & adr)
{
	return s_LagManager->GetPlayerLag(adr);
}

DETOUR_DECL_STATIC2(NET_LagPacket, bool, bool, newdata, _netpacket_t*, packet)
{

	// If "newdata" is true, packet points to a new packet of real data (which we could potentially lag)
	// If "newdata" is false, packet points to an empty, but initialized packet struct.
	// Returning true means we assert that "packet" is full of a consumable network packet.
	// Returning false means the consumer should ignore packet, there is no packet to consume yet.
	if (newdata) {
		const float lagTime = getLagPacketMs(packet->from);
		if (lagTime > 0.0) {
			// g_pSM->LogError(myself, "Lagging packet on socket %d for %fms", packet->source, lagTime / 2.0);
			s_LagSystem->LagPacket(packet, lagTime / 2.0);
		}
		else
		{
			// TODO: Flush lagged packets if the lag time is reduced or removed?
			return true;
		}
	}

	return s_LagSystem->GetNextPacket(packet->source, packet);
}

void SockAddrToNetAdr(const struct sockaddr *s, dumb_netadr_s *a);

DETOUR_DECL_STATIC6(DTR_NET_SendToImpl, int, SOCKET, s, const char *, buf, int, len, const struct sockaddr *, to, int, tolen, int, iGameDataLength)
{
	if (g_pLagPackedSender->IsRunning() && iGameDataLength != NET_QUEUED_PACKET_THREAD_SEND_PACKET)
	{
		dumb_netadr_s adr;
		SockAddrToNetAdr(to, &adr);

		const float lagTime = getLagPacketMs(adr);
		if (lagTime > 0.0)
		{
			// Warning( "DTR_NET_SendToImpl: Lagging packet on socket %d for %fms (%d bytes)\n", soc, lagTime, len);
			g_pLagPackedSender->QueuePacket(NULL, s, buf, len, to, tolen, (uint32)(lagTime / 2.0));
			return len;
		}
	}

	if (iGameDataLength == NET_QUEUED_PACKET_THREAD_SEND_PACKET)
		iGameDataLength = -1;

	return DTR_NET_SendToImpl_Actual(s, buf, len, to, tolen, iGameDataLength);
}

DETOUR_DECL_STATIC1(NET_ClearQueuedPacketsForChannel, void, INetChannel *, chan)
{
	g_pLagPackedSender->ClearQueuedPacketsForChannel(chan);
	return NET_ClearQueuedPacketsForChannel_Actual(chan);
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

	DSendToImpl = DETOUR_CREATE_STATIC(DTR_NET_SendToImpl, "NET_SendToImpl");
	if (DSendToImpl == NULL)
	{
		g_pSM->LogError(myself, "NET_SendToImpl detour could not be initialized - FeelsBadMan.");
		return false;
	}
	DSendToImpl->EnableDetour();

	DClearQueuedPacketsForChannel = DETOUR_CREATE_STATIC(NET_ClearQueuedPacketsForChannel, "NET_ClearQueuedPacketsForChannel");
	if (DClearQueuedPacketsForChannel == NULL)
	{
		g_pSM->LogError(myself, "NET_ClearQueuedPacketsForChannel detour could not be initialized - FeelsBadMan.");
		return false;
	}
	DClearQueuedPacketsForChannel->EnableDetour();

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
		s_LagSystem = NULL;
	}
	s_LagManager = NULL;
}

void SockAddrToNetAdr( const struct sockaddr *s, dumb_netadr_s *a )
{
	if (s->sa_family == AF_INET)
	{
		a->type = NA_IP;
		*(int *)&a->ip = ((struct sockaddr_in *)s)->sin_addr.s_addr;
		a->port = ((struct sockaddr_in *)s)->sin_port;
	}
}

