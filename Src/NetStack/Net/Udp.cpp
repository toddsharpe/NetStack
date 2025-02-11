#include "Net/Udp.h"
#include "Net/Socket.h"
#include "Net/NetIf.h"
#include "Core/Deserializer.h"
#include "Core/Serializer.h"

namespace Net::Udp
{
	void Receive(NetIf& net_if, Packet& packet)
	{
		_ASSERT(packet.length() >= sizeof(udp_hdr_t));
		Deserializer deser(packet.buffer(), packet.length());
		const udp_hdr_t udp_hdr =
		{
			.src_port = deser.read<uint16_t>(),
			.dst_port = deser.read<uint16_t>(),
			.length = deser.read<uint16_t>(),
			.checksum = deser.read<uint16_t>(),
		};
		packet.offset += sizeof(udp_hdr_t);

		//Update endpoints
		packet.dst.port = udp_hdr.dst_port;
		packet.src.port = udp_hdr.src_port;
		packet.proto = Protocol::Udp;

		net_if.socket.Receive(net_if, packet);
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		_ASSERT(packet.offset >= sizeof(udp_hdr_t));
		packet.offset -= sizeof(udp_hdr_t);

		//TODO(tsharpe): Finish
		const udp_hdr_t hdr =
		{
			.src_port = packet.src.port,
			.dst_port = packet.dst.port,
			.length = static_cast<uint16_t>(packet.length()),
			.checksum = 0x00
		};

		Serializer ser(packet.buffer(), packet.length());
		ser.write(hdr.src_port);
		ser.write(hdr.dst_port);
		ser.write(hdr.length);
		ser.write(hdr.checksum);
		
		packet.proto = Protocol::Udp;
		return net_if.ip.Send(net_if, packet);
	}
}
