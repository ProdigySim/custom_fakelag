#include <cstddef>
#include <cstring>
#include "LagSystem.h"

// uncomment for log statements
#include "extension.h"


void LagSystem::LagPacket(_netpacket_t* pPacket, float lagTime)
{
	auto newPacket = _netpacket_t(*pPacket);

	// CUSTOM!!!
	// Update received time to be the new time we want to read the packet
	newPacket.received = newPacket.received + (lagTime / 1000.0f);

	m_LagPackets[newPacket.source].add(newPacket);
}

bool LagSystem::GetNextPacket(int socket, _netpacket_t* destPacket)
{
	auto packetQueue = &m_LagPackets[socket];
	if (packetQueue->empty()) {
		// There are no waiting packets for this socket. pzpz.
		return false;
	}

	// Is it time to process this packet?
	if (packetQueue->peek().received > GetNetTime()) {
		g_pSM->LogError(myself, "Top packet is not due yet (%f > %f)", packetQueue->peek().received, GetNetTime());
		return false;
	}

	// It's time for this packet!
	g_pSM->LogError(myself, "It's time for a packet! (%f < %f)", packetQueue->peek().received, GetNetTime());

	// Unlink the top packet from the list. (pop)
	const _netpacket_t topPacket = packetQueue->popCopy();

	// Copy the packet contents to the net packet.
	//memcpy(&destPacket->from, &topPacket.from, sizeof(netadr_t));
	destPacket->from = topPacket.from;
	destPacket->pNext = NULL;
	destPacket->received = topPacket.received; // Custom: Keep same fake received time, instead of taking current net_time.
	destPacket->size = topPacket.size;
	destPacket->wiresize = topPacket.wiresize;
	destPacket->stream = topPacket.stream;
	memcpy(destPacket->data, topPacket.data, topPacket.size);


	// g_pSM->LogError(myself, "Finished copying packet of size %d from ip %08x", destPacket->received, destPacket->from.ip);
	return true;
}
