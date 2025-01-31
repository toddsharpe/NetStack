#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

namespace Net
{
	namespace Udp
	{
		struct udp_hdr_t
		{
			uint16_t src_port;
			uint16_t dst_port;
			uint16_t length;
			uint16_t checksum;
		};

		void Receive(NetIf& net_if, Packet& packet);
		bool Send(NetIf& net_if, Packet& packet);
	}

	const NetLayer UdpLayer = { Udp::Receive, Udp::Send, sizeof(Udp::udp_hdr_t) };
}
