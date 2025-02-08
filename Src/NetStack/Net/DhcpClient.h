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
			request_wait
		};

		state_t get_next_state();
		void dispatch_state();

		void dispatch_discover();
		void discover_offer_wait();

		state_t current_state;
	};
}
