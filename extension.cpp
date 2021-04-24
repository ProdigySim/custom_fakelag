/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "forwards.h"
#include "Net_LagPacket.h"

CustomFakelag g_Sample;		/**< Global singleton for extension's main interface */
extern sp_nativeinfo_t g_CFakeLagNatives[];
IGameConfig* g_pGameConf = NULL;

bool CustomFakelag::SDK_OnLoad(char* error, size_t maxlen, bool late) 
{
	// Load Gamedata
	char conf_error[255];
	if (!gameconfs->LoadGameConfigFile("custom_fakelag.games", &g_pGameConf, &conf_error[0], sizeof(conf_error)))
	{
		if (error)
		{
			ke::SafeSprintf(error, maxlen, "Could not read custom_fakelag.games: %s", &conf_error[0]);
		}
		return false;
	}

	// Find `net_time` address in memory
	double* pNetTime = NULL;
	if (!g_pGameConf->GetAddress("net_time", reinterpret_cast<void**>(&pNetTime))) {
		ke::SafeSprintf(error, maxlen, "Could not find net_time address in memory");
		return false;
	}

	m_LagManager = new PlayerLagManager(engine);

	// Initialize Detour System.
	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);


	// Detour NET_LagPacket()
	if (!LagDetour_Init(m_LagManager, pNetTime)) {
		ke::SafeSprintf(error, maxlen, "Could not detour Net_LagPacket.");
		return false;
	}

	
	sharesys->AddNatives(myself, g_CFakeLagNatives);
	sharesys->RegisterLibrary(myself, "custom_fakelag");
	return true;
}

void CustomFakelag::SDK_OnAllLoaded()
{
}

void CustomFakelag::SDK_OnUnload() {
	LagDetour_Shutdown();
	if (m_LagManager)
	{
		delete m_LagManager;
	}
	m_LagManager = NULL;
}


void CustomFakelag::SetPlayerLatency(int client, float lagTime)
{
	if (m_LagManager != NULL)
	{
		m_LagManager->SetPlayerLag(client, lagTime);
	}
}

float CustomFakelag::GetPlayerLatency(int client)
{
	if(m_LagManager != NULL)
	{
		return m_LagManager->GetPlayerLag(client);
	}
	return 0.0f;
}

// native void CFakeLag_SetPlayerLatency(int client, float lagTime)
cell_t CFakeLag_SetPlayerLatency(IPluginContext *pContext, const cell_t *params)
{
	int client = params[1];
	float lagTime = sp_ctof(params[2]);
	auto player = playerhelpers->GetGamePlayer(client);
	if (player == NULL) {
		return pContext->ThrowNativeError("Client index %d is not valid", client);
	}

	if(player->IsFakeClient())
	{
		return pContext->ThrowNativeError("Client index %d is a fake client and can't be lagged.", client);
	}
	g_Sample.SetPlayerLatency(client, lagTime);
	return 1;

}// native void CFakeLag_SetPlayerLatency(int client, float lagTime)
cell_t CFakeLag_GetPlayerLatency(IPluginContext *pContext, const cell_t *params)
{
	int client = params[1];
	auto player = playerhelpers->GetGamePlayer(client);
	if (player == NULL) {
		return pContext->ThrowNativeError("Client index %d is not valid", client);
	}

	if(player->IsFakeClient())
	{
		return pContext->ThrowNativeError("Client index %d is a fake client and can't be lagged.", client);
	}
	return sp_ftoc(g_Sample.GetPlayerLatency(client));
}


sp_nativeinfo_t g_CFakeLagNatives[] = 
{
	{"CFakeLag_SetPlayerLatency",			CFakeLag_SetPlayerLatency},
	{"CFakeLag_GetPlayerLatency",			CFakeLag_GetPlayerLatency},
	{NULL,							NULL}
};

SMEXT_LINK(&g_Sample);
