#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

namespace Net
{
	namespace Ethernet
	{
		static constexpr uint16_t eth_type_ipv4 = 0x0800;
		static constexpr uint16_t eth_type_arp = 0x0806;

		struct eth_mac_t
		{
			uint8_t bytes[6];
		};

		struct eth_hdr_t
		{
			eth_mac_t dst;
			eth_mac_t src;
			uint16_t type;
		};
		static_assert(sizeof(eth_hdr_t) < l2_res);

		void Receive(NetIf& net_if, Packet& packet);
		bool Send(NetIf& net_if, Packet& packet);

		typedef eth_mac_t addr_t;
	}
}
