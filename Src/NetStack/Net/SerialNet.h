#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

namespace Net
{
	namespace SerialNet
	{
		constexpr uint8_t serial_broadcast = 0xFF;

		struct serial_hdr_t
		{
			uint8_t dst;
			uint8_t src;
			uint16_t length;
			uint16_t crc;
		};
		static_assert(sizeof(serial_hdr_t) < l2_res);

		void Receive(NetIf& net_if, Packet& packet);
		bool Send(NetIf& net_if, Packet& packet);

		typedef uint8_t addr_t;
	}

	const NetLayer SerialNetLayer = { SerialNet::Receive, SerialNet::Send, sizeof(SerialNet::serial_hdr_t) };
}
