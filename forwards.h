#ifndef _INCLUDE_CUSTOM_FAKELAG_FORWARDS_H_
#define _INCLUDE_CUSTOM_FAKELAG_FORWARDS_H_


bool CreateNetLagPacketDetour();
void RemoveNetLagPacketDetour();
extern IForward* g_fwdLagPacket = NULL;

#endif // _INCLUDE_CUSTOM_FAKELAG_FORWARDS_H_
