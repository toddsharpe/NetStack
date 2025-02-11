#include "Net/Icmp.h"
#include "Net/NetIf.h"
#include "Core/Deserializer.h"
#include "Core/Serializer.h"

namespace Net::Icmp
{
	namespace
	{
		static uint8_t GetType(const Protocol proto)
		{
			switch (proto)
			{
				case Protocol::EchoReply:
					return icmp_type_echo_reply;

				case Protocol::EchoRequest:
					return icmp_type_echo_request;

				default:
					_ASSERT(false);
					return 0;
			}
		}
	}
	
	namespace EchoReply
	{
		static void Receive(NetIf& net_if, Packet& packet)
		{
			if (packet.length() < sizeof(echo_hdr_t))
			{
				packet.release();
				return;
			}
			
			Deserializer deser(packet.buffer(), packet.length());
			const echo_hdr_t hdr =
			{
				.id = deser.read<uint16_t>(),
				.seq_num = deser.read<uint16_t>(),
			};
			packet.offset += sizeof(echo_hdr_t);
			packet.proto = Protocol::EchoReply;

			//TODO: reply
			_ASSERT(false);
		}
	}

	namespace EchoRequest
	{
		static void Receive(NetIf& net_if, Packet& packet)
		{
			if (packet.length() < sizeof(echo_hdr_t))
			{
				packet.release();
				return;
			}
			
			Deserializer deser(packet.buffer(), packet.length());
			const echo_hdr_t hdr =
			{
				.id = deser.read<uint16_t>(),
				.seq_num = deser.read<uint16_t>(),
			};
			packet.offset += sizeof(echo_hdr_t);
			packet.proto = Protocol::EchoRequest;

			//Allocate packet to send
			Packet* tx_packet = net_if.driver.TxAlloc();
			_ASSERT(tx_packet);

			tx_packet->offset = packet.offset - sizeof(echo_hdr_t);
			tx_packet->dst = packet.src;
			tx_packet->src = packet.dst;

			//Reflect id, seq num, and payload
			Serializer ser(tx_packet->buffer(), tx_packet->length());
			ser.write(hdr.id);
			ser.write(hdr.seq_num);
			ser.write_from(deser.peek(), deser.remaining());
			tx_packet->count = ser.bytes() + tx_packet->offset;

			//Release received packet
			packet.release();

			//Send response
			tx_packet->proto = Protocol::EchoReply;
			net_if.icmp.Send(net_if, *tx_packet);
		}
	}
	
	void Receive(NetIf& net_if, Packet& packet)
	{
		if (packet.length() < sizeof(icmp_hdr_t))
		{
			packet.release();
			return;
		}
		
		Deserializer deser(packet.buffer(), packet.length());
		const icmp_hdr_t hdr =
		{
			.type = deser.read<uint8_t>(),
			.code = deser.read<uint8_t>(),
			.checksum = deser.read<uint16_t>(),
		};
		packet.offset += sizeof(icmp_hdr_t);
		packet.proto = Protocol::Icmp;

		switch (hdr.type)
		{
			case icmp_type_echo_reply:
				EchoReply::Receive(net_if, packet);
				break;

			case icmp_type_dest_unreachable:
				_ASSERT(false);
				break;

			case icmp_type_echo_request:
				EchoRequest::Receive(net_if, packet);
				break;

			default:
				_ASSERT(false);
				break;
		}
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		if (packet.offset < sizeof(icmp_hdr_t))
		{
			packet.release();
			return false;
		}

		packet.offset -= sizeof(icmp_hdr_t);

		const icmp_hdr_t hdr =
		{
			.type = GetType(packet.proto),
			.code = 0,
			.checksum = 0
		};

		Serializer ser(packet.buffer(), packet.length());
		ser.write(hdr.type);
		ser.write(hdr.code);

		const uint16_t expected_checksum = Net::checksum(packet.buffer(), packet.length());
		ser.write(expected_checksum);

		packet.proto = Protocol::Icmp;
		return net_if.ip.Send(net_if, packet);
	}
}
