#pragma once

#include "Net/Dhcp.h"
#include "Core/Deserializer.h"
#include "Core/Serializer.h"
#include <crtdbg.h>

namespace Net
{
	class DhcpClient
	{
	public:
		DhcpClient();

		void Start();
		void Dispatch();

	private:
		static uint32_t transaction_id;
		static constexpr size_t options_resv = 128;

		enum state_t
		{
			idle,
			discover,
			offer_wait,
			request,
			ack_wait,
			finished
		};

		state_t get_next_state() const;
		void dispatch_state();

		void dispatch_discover();
		void dispatch_offer_wait();
		void dispatch_request();
		void dispatch_ack_wait();

		state_t current_state;

		//dhcp info
		ipv4_addr_t your_ip;
		ipv4_addr_t server_ip;
	};
}
