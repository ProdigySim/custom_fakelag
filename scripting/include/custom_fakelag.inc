#if defined _custom_fakelag_included
#endinput
#endif
#define _custom_fakelag_included

/**
 * @brief Called whenever a packet is received in order to allow custom latency.
 *
 * @param lagTime Amount of time to lag the packet, in milliseconds..
 * @param ip		IP Address source of the packet
 */
forward void CustomLag_OnLagPacket(float & lagTime, int ip);


public Extension __ext_custom_fakelag =
{
	name = "custom_fakelag",
	file = "custom_fakelag.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};