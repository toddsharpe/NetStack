#include "Net/Packet.h"
#include "Net/SerialNet.h"
#include "Test/NullMac.h"
#include "Net/NetIf.h"

using namespace Net;

static constexpr uint8_t rx_idle[] = { 0x01, 0x00, 0x00, 0x06, 0xAA, 0xBB };
static constexpr uint8_t rx_frame[] = { 0x01, 0x00, 0x00, 0x0A, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
static constexpr uint8_t rx_data[] = { 0xCC, 0xDD, 0xEE, 0xFF };
static constexpr uint8_t rx_peer_frame[] = { 0x02, 0x00, 0x00, 0x0A, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

namespace
{
	Packet rx_packet;
	Packet tx_packet;
	
	void Receive(Net::NetIf& net_if, Packet& packet)
	{
		rx_packet = packet;
	}

	bool Send(Net::NetIf& net_if, Packet& packet)
	{
		tx_packet = packet;
		return true;
	}

	bool Forward(Net::NetIf& net_if, Packet& packet)
	{
		return false;
	}

	void Clear()
	{
		rx_packet.release();
		tx_packet.release();
	}

	NetLayer test_ip(Receive, Send, size_t(0));

	NullMac driver;
}

void Test_SerialNet()
{
	/*
	 * Create interface.
	 */
	NetBindings test_bindings =
	{
		.ip = { Receive, Send},
	};

	NetIf net_if(driver, NetLayer(), "Test", test_bindings);
	net_if.config.serial_addr = 0x01;

	/*
	 * Receive idle.
	 */
	{
		Packet packet;
		CopyTo(packet, rx_idle);
		SerialNet::Receive(net_if, packet);
		_ASSERT(rx_packet.count == 0);
		Clear();
	}

	/*
	 * Receive frame.
	 */
	{
		Packet packet;
		CopyTo(packet, rx_frame);
		SerialNet::Receive(net_if, packet);
		_ASSERT(rx_packet.length() == sizeof(rx_data));
		_ASSERT(memcmp(rx_packet.buffer(), rx_data, sizeof(rx_data)) == 0);
		Clear();
	}

	/*
	 * Receive peer frame and drop.
	 */
	{
		Packet packet;
		CopyTo(packet, rx_peer_frame);
		SerialNet::Receive(net_if, packet);
		_ASSERT(rx_packet.count == 0);
		Clear();
	}
}
