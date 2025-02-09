#include "Net/Socket.h"
#include "Net/Router.h"
#include "Net/NetIf.h"

namespace
{
	struct packet_entry_t
	{
		packet_entry_t() : packet(), in_use()
		{

		}

		void release()
		{
			if (packet)
				packet->release();
			in_use = false;
		}

		Net::Packet* packet;
		bool in_use;
	};

	std::array<packet_entry_t, 8> rx_packets;

	size_t udp_header_size(Net::NetIf& net_if)
	{
		return net_if.eth.HeaderSize + net_if.ip.HeaderSize + net_if.udp.HeaderSize;
	}
}

namespace Net::Socket
{
	void Receive(NetIf& net_if, Packet& packet)
	{
		for (packet_entry_t& rx : rx_packets)
		{
			if (rx.in_use)
				continue;

			rx.packet = &packet;
			rx.in_use = true;
			break;
		}
	}

	bool ReadUdp(const endpoint_t dst, endpoint_t& src, void* const buffer, const size_t length, size_t& bytes_read)
	{
		for (packet_entry_t& entry : rx_packets)
		{
			if (!entry.in_use)
				continue;

			if (entry.packet->dst != dst)
				continue;

			if (length < entry.packet->count)
				return false;

			src = entry.packet->src;
			memcpy(buffer, entry.packet->buffer(), entry.packet->length());
			
			entry.release();
			return true;
		}

		return false;
	}
	
	//Note(tsharpe): Ideally this method would copy user data to one buffer and pass down multiple stacks if needed.
	//However, without scatter/gather for headers (which will be interface dependent), copying is necessary
	bool SendUdp(const endpoint_t dst, const uint16_t src_port, const void* const buffer, const size_t length)
	{
		const size_t if_idx = Router::Resolve(dst);
		NetIf& net_if = Router::GetInterface(if_idx);

		//Get packet from driver
		Packet* packet = net_if.driver.TxAlloc();
		if (!packet)
			return false;

		//Write data to packet
		const size_t offset = udp_header_size(net_if);
		memcpy(&packet->data[offset], buffer, length);
		packet->count = offset + length;
		packet->offset = offset;

		//Populate packet fields
		packet->dst = dst;
		packet->src.port = src_port != 0 ? src_port : (rand() % UINT16_MAX);

		return net_if.udp.Send(net_if, *packet);
	}
}
