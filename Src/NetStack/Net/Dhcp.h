#pragma once

#include "Net/Net.h"
#include "Net/Packet.h"
#include "Net/NetTypes.h"
#include "Core/Deserializer.h"
#include <cstdint>

namespace Net
{
	namespace Dhcp
	{
		static constexpr uint32_t magic_cookie = 0x63825363;

		static constexpr uint8_t opcode_request = 1;
		static constexpr uint8_t opcode_response = 2;

		static constexpr uint8_t options_subnet = 1;
		static constexpr uint8_t options_router = 3;
		static constexpr uint8_t options_dns = 6;
		static constexpr uint8_t options_requested_ip = 50;
		static constexpr uint8_t options_message_type = 53;
		static constexpr uint8_t options_dhcp_server = 54;
		static constexpr uint8_t options_param_req_list = 55;
		static constexpr uint8_t options_end = 255;

		//Message types (Option 53)
		static constexpr uint8_t dhcp_discover = 0x01;
		static constexpr uint8_t dhcp_offer = 0x02;
		static constexpr uint8_t dhcp_request = 0x03;
		static constexpr uint8_t dhcp_ack = 0x05;

		static constexpr uint8_t hardware_type_ethernet = 0x01;
		static constexpr uint8_t haddr_len = 0x06;

		static constexpr uint16_t server_port = 67;
		static constexpr uint16_t client_port = 68;

		struct dhcp_hdr_t
		{
			uint8_t opcode;
			uint8_t hardware_type;
			uint8_t haddr_length;
			uint8_t hops;
			uint32_t transaction_id;
			uint16_t seconds;
			uint16_t flags;
			ipv4_addr_t client_addr;
			ipv4_addr_t your_addr;
			ipv4_addr_t server_addr;
			ipv4_addr_t gateway_addr;
			uint8_t client_haddr[16];
			uint8_t server_name[64];
			uint8_t file[128];
			uint32_t magic_cookie;
		};

		template <typename T>
		inline T GetOption(const uint8_t* const options, const size_t options_length, const uint8_t code)
		{
			for (size_t i = 0; i < options_length - 1; i++)
			{
				if (options[i] != code || options[i + 1] != sizeof(T))
					continue;

				Deserializer deser(&options[i + 2], sizeof(T));
				return deser.read<T>();
			}
			return 0;
		}

		void GetOption(const uint8_t* const options, const size_t options_length, const uint8_t code, void* const dst, const size_t length);
	}
}
