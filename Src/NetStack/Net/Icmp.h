#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

namespace Net
{
	namespace Icmp
	{
		static constexpr uint8_t icmp_type_echo_reply = 0x00;
		static constexpr uint8_t icmp_type_dest_unreachable = 0x03;
		static constexpr uint8_t icmp_type_echo_requests = 0x08;

		struct icmp_hdr_t
		{
			uint8_t type;
			uint8_t code;
			uint16_t checksum;
		};

		struct echo_hdr_t
		{
			uint16_t id;
			uint16_t seq_num;
		};

		void Receive(NetIf& net_if, Packet& packet);
		bool Send(NetIf& net_if, Packet& packet);
	}

	const NetLayer IcmpLayer = { Icmp::Receive, Icmp::Send, sizeof(Icmp::icmp_hdr_t) };
}
