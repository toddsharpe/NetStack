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
	}

	const NetLayer IPv4Layer = { IPv4::Receive, IPv4::Send, sizeof(IPv4::ipv4_hdr_t) };
}
