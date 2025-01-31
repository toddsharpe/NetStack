#pragma once

#include "Net/Net.h"
#include "Net/Packet.h"
#include "Net/NetTypes.h"

#include <cstdint>
#include <array>

namespace Net
{
	namespace Socket
	{
		void Receive(NetIf& net_if, Packet& packet);

		bool SendUdp(const endpoint_t dst, const uint16_t src_port, const void* const buffer, const size_t length);
		bool ReadUdp(const endpoint_t dst, endpoint_t& src, void* const buffer, const size_t length, size_t& bytes_read);
	}
	const NetLayer SocketLayer = { Socket::Receive, nullptr };
}
