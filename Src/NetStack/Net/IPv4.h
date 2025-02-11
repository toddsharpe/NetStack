#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"
#include <string>

namespace Net
{
	namespace IPv4
	{
		static constexpr uint8_t ip_proto_icmp = 0x01;
		static constexpr uint8_t ip_proto_igmp = 0x02;
		static constexpr uint8_t ip_proto_tcp = 0x06;
		static constexpr uint8_t ip_proto_udp = 0x11;

		constexpr uint8_t get_proto(Net::Protocol proto)
		{
			switch (proto)
			{
				case Protocol::None:
					return 0;

				case Protocol::Udp:
					return ip_proto_udp;

				case Protocol::Icmp:
					return ip_proto_icmp;

				default:
					return 0;
			}
		}

		struct ipv4_hdr_t
		{
			uint8_t ihl_version;
			uint8_t dscp;
			uint16_t length;
			uint16_t id;
			uint16_t fragment_flags;
			uint8_t ttl;
			uint8_t proto;
			uint16_t checksum;
			ipv4_addr_t src;
			ipv4_addr_t dst;
		};

		struct ipv4_mc_route_t
		{
			ipv4_addr_t group;
			NetIf* dst;
		};

		void Receive(NetIf& net_if, Packet& packet);
		bool Send(NetIf& net_if, Packet& packet);

		struct data_t
		{
			data_t() : mc_groups(), mc_routes(), addr(), subnet_mask(Net::subnet_32), ip_forwarding(true), rx_dropped(), rx_accepted(), rx_unicast(), rx_multicast(), rx_broadcast()
			{

			}

			bool is_joined(const ipv4_addr_t mc_group) const
			{
				for (const ipv4_addr_t& addr : mc_groups)
				{
					if (addr == mc_group)
						return true;
				}
				return false;
			}

			/*
			 * Config.
			 */
			std::array<ipv4_addr_t, 4> mc_groups;
			std::array<IPv4::ipv4_mc_route_t, 4> mc_routes;
			ipv4_addr_t addr;
			ipv4_addr_t subnet_mask;
			bool ip_forwarding;

			/*
			 * Stats.
			 */
			uint32_t rx_dropped;
			uint32_t rx_accepted;
			uint32_t rx_unicast;
			uint32_t rx_multicast;
			uint32_t rx_broadcast;
		};
	}

	const NetLayer IPv4Layer = { IPv4::Receive, IPv4::Send, sizeof(IPv4::ipv4_hdr_t) };
}
