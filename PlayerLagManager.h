#pragma once
#include <mathlib.h>
#include <eiface.h>
#include <inetchannel.h>
#include <amtl/am-hashmap.h>



struct NetAdrHashPolicy_s {
	static uint32_t hash(const netadr_t& value)
	{
		return *(uint32_t*)value.ip;
	}

	static bool matches(const netadr_t& value, const netadr_t& key)
	{
		return *(uint32_t*)(value.ip) == *(uint32_t*)(key.ip) && value.port == key.port && value.type == key.type;
	}
};

class PlayerLagManager
{
private:
	IVEngineServer* m_pEngine;
	ke::HashMap<netadr_t, float, NetAdrHashPolicy_s> m_LagTimes;

	const netadr_t& GetClientNetAdr(int client) const;

public:
	PlayerLagManager(IVEngineServer* engine) : m_pEngine(engine) {
		m_LagTimes.init(32);
	}

	void SetPlayerLag(int client, float lagTime);

	float GetPlayerLag(int client) const;

	float GetPlayerLag(const netadr_t& netadr) const;
};
