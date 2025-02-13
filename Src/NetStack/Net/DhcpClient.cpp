#include "Net/DhcpClient.h"
#include "Net/Socket.h"

namespace Net
{
	uint32_t DhcpClient::transaction_id = 0;
	
	DhcpClient::DhcpClient() : current_state()
	{

	}

	void DhcpClient::Start()
	{
		
	}

	void DhcpClient::Dispatch()
	{
		const state_t next_state = get_next_state();
		if (current_state != next_state)
		{
			current_state = next_state;
		}

		dispatch_state();
	}

	DhcpClient::state_t DhcpClient::get_next_state() const
	{
		switch (current_state)
		{
			case state_t::idle:
				return state_t::discover;

			case state_t::discover:
				return state_t::offer_wait;

			case state_t::offer_wait:
				return state_t::request;

			case state_t::request:
				return state_t::ack_wait;

			case state_t::ack_wait:
				return state_t::finished;

			default:
				_ASSERT(false);
				return current_state;
		}
	}

	void DhcpClient::dispatch_state()
	{
		switch (current_state)
		{
			case state_t::idle:
				break;

			case state_t::discover:
				dispatch_discover();
				break;

			case state_t::offer_wait:
				dispatch_offer_wait();
				break;

			case state_t::request:
				dispatch_request();
				break;

			case state_t::ack_wait:
				dispatch_ack_wait();
				break;

			default:
				_ASSERT(false);
				return;
		}
	}

	void DhcpClient::dispatch_discover()
	{
		const uint8_t options[] =
		{
			Dhcp::options_message_type, 0x01, Dhcp::dhcp_discover,
			Dhcp::options_end
		};
		const Dhcp::dhcp_hdr_t hdr =
		{
			.opcode = Dhcp::opcode_request,
			.hardware_type = Dhcp::hardware_type_ethernet,
			.haddr_length = Dhcp::haddr_len,
			.hops = 0,
			.transaction_id = transaction_id,
			.seconds = 0,
			.flags = 0,
			.client_addr = {},
			.your_addr = {},
			.server_addr = {},
			.gateway_addr = {},
			.client_haddr = { },
			.server_name = { },
			.file = {},
			.magic_cookie = Dhcp::magic_cookie,
		};

		//TODO(tsharpe): Populate chaddr

		uint8_t packet[sizeof(Dhcp::dhcp_hdr_t) + options_resv] = {};
		Serializer ser(packet, sizeof(packet));
		ser.write(hdr.opcode);
		ser.write(hdr.hardware_type);
		ser.write(hdr.haddr_length);
		ser.write(hdr.hops);
		ser.write(hdr.transaction_id);
		ser.write(hdr.seconds);
		ser.write(hdr.flags);
		ser.write(hdr.client_addr.addr);
		ser.write(hdr.your_addr.addr);
		ser.write(hdr.server_addr.addr);
		ser.write(hdr.gateway_addr.addr);
		ser.write_from(hdr.client_haddr, sizeof(hdr.client_haddr));
		ser.write_from(hdr.server_name, sizeof(hdr.server_name));
		ser.write_from(hdr.file, sizeof(hdr.file));
		ser.write(hdr.magic_cookie);
		ser.write_from(options, sizeof(options));

		Socket::SendUdp({ Net::broadcast, Dhcp::server_port }, Dhcp::client_port, packet, ser.bytes());
	}

	void DhcpClient::dispatch_offer_wait()
	{
		uint8_t buffer[sizeof(Dhcp::dhcp_hdr_t) + options_resv] = {};
		Net::endpoint_t src = {};
		size_t bytes_read = 0;
		const bool received = Socket::ReadUdp({ Net::broadcast, Dhcp::client_port }, src, buffer, sizeof(buffer), bytes_read);
		if (!received)
			return;

		Deserializer deser(buffer, sizeof(buffer));
		Dhcp::dhcp_hdr_t hdr =
		{
			.opcode = deser.read<uint8_t>(),
			.hardware_type = deser.read<uint8_t>(),
			.haddr_length = deser.read<uint8_t>(),
			.hops = deser.read<uint8_t>(),
			.transaction_id = deser.read<uint32_t>(),
			.seconds = deser.read<uint16_t>(),
			.flags = deser.read<uint16_t>(),
			.client_addr = {.addr = deser.read<uint32_t>() },
			.your_addr = {.addr = deser.read<uint32_t>() },
			.server_addr = {.addr = deser.read<uint32_t>() },
			.gateway_addr = {.addr = deser.read<uint32_t>() },
		};
		deser.read(hdr.client_haddr);
		deser.read(hdr.server_name);
		deser.read(hdr.file);
		hdr.magic_cookie = deser.read<uint32_t>();

		_ASSERT(hdr.magic_cookie == Dhcp::magic_cookie);

		const uint8_t msg_type = Dhcp::GetOption<uint8_t>(deser.peek(), deser.remaining(), Dhcp::options_message_type);
		if (msg_type != Dhcp::dhcp_offer)
			return;

		const Net::ipv4_addr_t router =
		{
			.addr = Dhcp::GetOption<uint32_t>(deser.peek(), deser.remaining(), Dhcp::options_router)
		};

		const Net::ipv4_addr_t server =
		{
			.addr = Dhcp::GetOption<uint32_t>(deser.peek(), deser.remaining(), Dhcp::options_dhcp_server)
		};

		//Save off
		your_ip = hdr.your_addr;
		server_ip = hdr.server_addr != Net::empty ? hdr.server_addr : server;
	}

	void DhcpClient::dispatch_request()
	{
		const uint8_t options[] =
		{
			Dhcp::options_message_type, 0x01, Dhcp::dhcp_request,
			Dhcp::options_requested_ip, 0x04, your_ip.bytes[3], your_ip.bytes[2], your_ip.bytes[1], your_ip.bytes[0],
			Dhcp::options_dhcp_server, 0x04, server_ip.bytes[3], server_ip.bytes[2], server_ip.bytes[1], server_ip.bytes[0],
			Dhcp::options_end
		};
		const Dhcp::dhcp_hdr_t hdr =
		{
			.opcode = Dhcp::opcode_request,
			.hardware_type = Dhcp::hardware_type_ethernet,
			.haddr_length = Dhcp::haddr_len,
			.hops = 0,
			.transaction_id = transaction_id,
			.seconds = 0,
			.flags = 0,
			.client_addr = {},
			.your_addr = your_ip,
			.server_addr = server_ip,
			.gateway_addr = {},
			.client_haddr = { },
			.server_name = { },
			.file = {},
			.magic_cookie = Dhcp::magic_cookie,
		};

		//TODO(tsharpe): Populate chaddr

		uint8_t packet[sizeof(Dhcp::dhcp_hdr_t) + options_resv] = {};
		Serializer ser(packet, sizeof(packet));
		ser.write(hdr.opcode);
		ser.write(hdr.hardware_type);
		ser.write(hdr.haddr_length);
		ser.write(hdr.hops);
		ser.write(hdr.transaction_id);
		ser.write(hdr.seconds);
		ser.write(hdr.flags);
		ser.write(hdr.client_addr.addr);
		ser.write(hdr.your_addr.addr);
		ser.write(hdr.server_addr.addr);
		ser.write(hdr.gateway_addr.addr);
		ser.write_from(hdr.client_haddr, sizeof(hdr.client_haddr));
		ser.write_from(hdr.server_name, sizeof(hdr.server_name));
		ser.write_from(hdr.file, sizeof(hdr.file));
		ser.write(hdr.magic_cookie);
		ser.write_from(options, sizeof(options));

		Socket::SendUdp({ Net::broadcast, Dhcp::server_port }, Dhcp::client_port, packet, ser.bytes());
	}

	void DhcpClient::dispatch_ack_wait()
	{
		uint8_t buffer[sizeof(Dhcp::dhcp_hdr_t) + options_resv] = {};
		Net::endpoint_t src = {};
		size_t bytes_read = 0;
		const bool received = Socket::ReadUdp({ Net::broadcast, Dhcp::client_port }, src, buffer, sizeof(buffer), bytes_read);
		if (!received)
			return;

		Deserializer deser(buffer, sizeof(buffer));
		Dhcp::dhcp_hdr_t hdr =
		{
			.opcode = deser.read<uint8_t>(),
			.hardware_type = deser.read<uint8_t>(),
			.haddr_length = deser.read<uint8_t>(),
			.hops = deser.read<uint8_t>(),
			.transaction_id = deser.read<uint32_t>(),
			.seconds = deser.read<uint16_t>(),
			.flags = deser.read<uint16_t>(),
			.client_addr = {.addr = deser.read<uint32_t>() },
			.your_addr = {.addr = deser.read<uint32_t>() },
			.server_addr = {.addr = deser.read<uint32_t>() },
			.gateway_addr = {.addr = deser.read<uint32_t>() },
		};
		deser.read(hdr.client_haddr);
		deser.read(hdr.server_name);
		deser.read(hdr.file);
		hdr.magic_cookie = deser.read<uint32_t>();

		_ASSERT(hdr.magic_cookie == Dhcp::magic_cookie);

		const uint8_t msg_type = Dhcp::GetOption<uint8_t>(deser.peek(), deser.remaining(), Dhcp::options_message_type);
		if (msg_type != Dhcp::dhcp_ack)
			return;
	}
}
