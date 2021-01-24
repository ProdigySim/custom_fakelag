#include <inetchannel.h>
#include <tier1/netadr.h>

// pointer to the `net_time` global variable.
double* g_pNetTime = NULL;

// This is only 3 in L4D2, but it's listed as 5 in VSES 2007 source. Doesn't really hurt to have extra empty slots....
#define MAX_SOCKETS 6
static netpacket_t* s_pLagData[MAX_SOCKETS] = { NULL };  // List of lagged packet structures.


void AddToLagged(netpacket_t* pPacket, float lagTime) {
	netpacket_t* newPacket = new netpacket_t;

	(*newPacket) = (*pPacket);
	newPacket->data = new unsigned char[pPacket->size];
	// TODO: Pick a different memcpy?
	memcpy(newPacket->data, pPacket->data, pPacket->size);
	
	// CUSTOM!!!
	// Update received time to be the new time we want to read the packet
	newPacket->received = newPacket->received + (lagTime / 1000.0f);

	netpacket_t** pList = &s_pLagData[newPacket->source];
	if (*pList == NULL) {
		// No packets on this list, update the HEAD.
		*pList = newPacket;
	}
	else {
		netpacket_t* next = *pList;

		if (next->received > newPacket->received) {
			// This packet will be consumed before the top of the stack, put it on the front.
			newPacket->pNext = next;
			*pList = newPacket;
		}
		else {
			// Find the correct location in this list to place the packet.
			// Optimization: This could be a min-heap structure, to reduce the traversal time to lg(n).
			netpacket_t* prev;
			do {
				prev = next;
				next = prev->pNext;
			} while (next != NULL && next->received < newPacket->received);

			// prev "arrives" sooner than newPacket.
			// next "arrives" later than newPacket, or is NULL.
			// Place newpacket between the two.
			prev->pNext = newPacket;
			newPacket->pNext = next;
		}
	}
}

bool GetNextPacket(int socket, netpacket_t * destPacket) {
	netpacket_t* pTopPacket = s_pLagData[socket];

	if (!pTopPacket) {
		// There are no waiting packets for this socket. pzpz.
		return false;
	}

	// Is it time to process this packet?
	if (pTopPacket->received > getNetTime()) {
		return false;
	}

	// It's time for this packet!

	// Unlink the top packet from the list. (pop)
	s_pLagData[socket] = pTopPacket->pNext;

	// Copy the packet contents to the net packet.
	destPacket->from = pTopPacket->from;
	destPacket->pNext = NULL;
	destPacket->received = pTopPacket->received; // Custom: Keep same fake received time, instead of taking current net_time.
	destPacket->size = pTopPacket->size;
	destPacket->wiresize = pTopPacket->wiresize;
	destPacket->stream = pTopPacket->stream;
	memcpy(destPacket->data, pTopPacket->data, pTopPacket->size);

	return true;
}

void SetNetTimePtr(double* net_time) {
	g_pNetTime = net_time;
}

double getNetTime() {
	return g_pNetTime == NULL ? 0.0f : *g_pNetTime;
}
