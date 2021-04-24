#ifndef _CUSTOM_FAKELAG_LAG_SYSTEM_H_
#define _CUSTOM_FAKELAG_LAG_SYSTEM_H_
#include <amtl/am-priority-queue.h>
#include "net_structures.h"


// This is only 3 in L4D2, but it's listed as 5 in VSES 2007 source. Doesn't really hurt to have extra empty slots....
#define MAX_SOCKETS 6

struct PacketEarlier {
	constexpr bool operator ()(const _netpacket_t& left, const _netpacket_t& right) const {
		return left.received < right.received;
	}
};
class LagSystem {
private:
	ke::PriorityQueue<_netpacket_t, PacketEarlier> m_LagPackets[MAX_SOCKETS];
	const double* m_pNetTime;

	inline double GetNetTime() { return *m_pNetTime; }
public:
	LagSystem(const double* pNetTime) : m_pNetTime(pNetTime) {
		assert(pNetTime != NULL);
	}
	void LagPacket(_netpacket_t* pPacket, float lagTime);
	bool GetNextPacket(int socket, _netpacket_t* destPacket);
};

#endif // _CUSTOM_FAKELAG_LAG_SYSTEM_H_