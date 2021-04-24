#include "PlayerLagManager.h"
#include "extension.h"

const dumb_netadr_t& PlayerLagManager::GetClientNetAdr(int client) const
{
	INetChannel* pNetChan = static_cast<INetChannel*>(m_pEngine->GetPlayerNetInfo(client));
	return pNetChan->GetRemoteAddress();
}

void PlayerLagManager::SetPlayerLag(int client, float lagTime)
{
	auto netadr = GetClientNetAdr(client);
	auto i = m_LagTimes.findForAdd(netadr);
	if (!i.found())
	{
		m_LagTimes.add(i, netadr);
	}
	i->value = lagTime;
	g_pSM->LogMessage(myself, "Lagging player index %d with net address %d.%d.%d.%d:%d for %.01fms", client, netadr.ip[0],netadr.ip[1],netadr.ip[2],netadr.ip[3],netadr.port, lagTime);
}

float PlayerLagManager::GetPlayerLag(int client) const
{
	auto netadr = GetClientNetAdr(client);
	return GetPlayerLag(netadr);
}

float PlayerLagManager::GetPlayerLag(const dumb_netadr_t& netadr) const
{
	auto i = m_LagTimes.find(netadr);
	if (!i.found())
	{
		// g_pSM->LogMessage(myself, "Lag not found for player @%d.%d.%d.%d:%d", netadr.ip[0],netadr.ip[1],netadr.ip[2],netadr.ip[3],netadr.port);
		return 0.0f;
	}
	// g_pSM->LogMessage(myself, "Lag for player @%d.%d.%d.%d:%d is %f", netadr.ip[0],netadr.ip[1],netadr.ip[2],netadr.ip[3],netadr.port, i->value);
	return i->value;
}
