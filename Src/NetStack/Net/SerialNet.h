#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

namespace Net
{
	namespace SerialNet
	{
		constexpr uint8_t broadcast = 0xFF;

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

		struct data_t
		{
			data_t() : addr(), filters(), is_slave(), rx_dropped(), rx_accepted(), rx_unicast(), rx_broadcast()
			{

			}
			
			/*
			 * Config.
			 */
			uint8_t addr;
			std::array<uint8_t, 4> filters;
			bool is_slave;

			/*
			 * Stats.
			 */
			uint32_t rx_dropped;
			uint32_t rx_accepted;
			uint32_t rx_unicast;
			uint32_t rx_broadcast;
		};
	}

	const NetLayer SerialNetLayer = { SerialNet::Receive, SerialNet::Send, sizeof(SerialNet::serial_hdr_t) };
}
