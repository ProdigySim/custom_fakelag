#include "PlayerLagManager.h"

const netadr_t& PlayerLagManager::GetClientNetAdr(int client) const
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
}

float PlayerLagManager::GetPlayerLag(int client) const
{
	auto netadr = GetClientNetAdr(client);
	return GetPlayerLag(netadr);
}

float PlayerLagManager::GetPlayerLag(const netadr_t& netadr) const
{
	auto i = m_LagTimes.find(netadr);
	if (!i.found())
	{
		return 0.0f;
	}
	return i->value;
}
