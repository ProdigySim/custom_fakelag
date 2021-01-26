#include <custom_fakelag>
#include <console>

public Plugin:myinfo = 
{
  name = "Per-Player Fakelag",
  author = "ProdigySim",
  description = "Hey you little shit I hope you like getting lagged for no reason",
  version = "any%",
  url = "https://github.com/ProdigySim/custom_fakelag"
};

Handle g_hLagTrie;

public OnPluginStart()
{
  g_hLagTrie = CreateTrie();

  RegAdminCmd("sm_fakelag", FakeLagCmd, view_as<int>(Admin_Config), "Set fake lag for a player");
  // DEBUG: See the value of s_FakeLag
  // RegAdminCmd("sm_printlag", PrintLagCmd, view_as<int>(Admin_Config), "Print Current FakeLag");
}


// void NET_LagPacket(bool newdata, netpacket_t * packet);
// netpacket_t: https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/net.h#L97-L108
// offset 0: netadr_t from
// netadr_t: https://github.com/VSES/SourceEngine2007/blob/master/src_main/public/tier1/netadr.h#L26-L68
// NET_LagPacket: https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/net_ws.cpp#L925
// public MRESReturn Detour_LagPacket(DHookReturn hReturn, DHookParam hParams)
// {
// 	// Step 1: Read from netpacket and filter to only ones from a given player
// 	// Step 2: set FakeLag static variable based on ip
  
// 	int ip = DHookGetParamObjectPtrVar(hParams, 2, 4, ObjectValueType_Int );
// 	static char ipString[32];
// 	Format(ipString, 31, "%d.%d.%d.%d", LOBYTE(ip), LOBYTE(ip >> 8), LOBYTE(ip >> 16), LOBYTE(ip >> 24));
// 	float lagAmount;
// 	if (GetTrieValue(g_hLagTrie, ipString, lagAmount) && lagAmount > 0) {
// 		g_PreviousFakeLag = view_as<float>(LoadFromAddress(g_pFakeLag, NumberType_Int32));
// 		StoreToAddress(g_pFakeLag, view_as<int>(lagAmount), NumberType_Int32)
    
// 		// PrintToServer("LAGGING PLAYER prevLag %f newLag %f", g_PreviousFakeLag, lagAmount);
// 		g_bIsOverridingLagNow = true;
// 	}

// 	return MRES_Handled;
// }

public void CustomLag_OnLagPacket(float & lagTime, int ip) {
  static char ipString[32];
  Format(ipString, 31, "%d.%d.%d.%d", LOBYTE(ip), LOBYTE(ip >> 8), LOBYTE(ip >> 16), LOBYTE(ip >> 24));
  float lagAmount;
	if (GetTrieValue(g_hLagTrie, ipString, lagAmount) && lagAmount > 0) {
    lagTime = lagAmount;
	}
  // PrintToServer("User IP %d.%d.%d.%d lag: %f", LOBYTE(ip), LOBYTE(ip >> 8), LOBYTE(ip >> 16), LOBYTE(ip >> 24), lagTime);
}

public Action FakeLagCmd(int client, int args) {
  if(args < 1) {
    ReplyToCommand(client, "Usage: sm_fakelag <target> <millseconds>");
    return Plugin_Handled;
  }
  char targetStr[256];
  GetCmdArg(1, targetStr, sizeof(targetStr))
  int target = FindTarget(client, targetStr, true);
  if(target < 0) {
    ReplyToCommand(client, "Unable to find target \"%s\"");
    return Plugin_Handled;
  }
  char name[256];
  GetClientName(target, name, sizeof(name));
  if(!IsClientInGame(target)) {
    ReplyToCommand(client, "Player %s is not in game yet.", name);
    return Plugin_Handled;
  }
  char ip[32];
  GetClientIP(target, ip, sizeof(ip));

  int lagAmount = GetCmdArgInt(2);
  if(lagAmount == 0) {
    RemoveFromTrie(g_hLagTrie, ip);
    ReplyToCommand(client, "Turned off Fake Lag for player %s", name);
    return Plugin_Handled;
  } else {
    float flAmount = lagAmount * 1.0;
    SetTrieValue(g_hLagTrie, ip, flAmount);
    ReplyToCommand(client, "Set Fake Lag for player %s to %dms", name, lagAmount);
    PrintToServer("FakeLag ON %s (ip: %s)", name, ip);
  }
  return Plugin_Handled;
}


// DEBUG: See the value of s_FakeLag
// public Action PrintLagCmd(int client, int args) {
// 	float fakelag = view_as<float>(LoadFromAddress(g_pFakeLag, NumberType_Int32));
// 	ReplyToCommand(client, "Current Fake Lag is %f", fakelag);

// 	return Plugin_Handled;
// }

int LOBYTE(int b) {
  return (b) & 0xFF;
}




stock int GetCmdArgInt(int argnum) {
    char str[12];
    GetCmdArg(argnum, str, sizeof(str));

    return StringToInt(str);
}