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

			bool operator==(const eth_mac_t& other) const
			{
				return memcmp(bytes, other.bytes, sizeof(eth_mac_t::bytes)) == 0;
			}

			bool operator!=(const eth_mac_t& other) const
			{
				return !(*this == other);
			}
		};
		static constexpr eth_mac_t broadcast = { .bytes = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

		struct eth_hdr_t
		{
			eth_mac_t dst;
			eth_mac_t src;
			uint16_t type;
		};
		static_assert(sizeof(eth_hdr_t) < l2_res);

		void Receive(NetIf& net_if, Packet& packet);
		bool Send(NetIf& net_if, Packet& packet);

		struct data_t
		{
			data_t() : addr(), filters()
			{

			}
			
			eth_mac_t addr;
			std::array<eth_mac_t, 4> filters;
		};
	}
}
