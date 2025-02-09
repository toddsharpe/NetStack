#include "Net/IPv4.h"
#include "Net/Udp.h"
#include "Net/Icmp.h"
#include "Net/Socket.h"
#include "Core/Deserializer.h"
#include "Core/Serializer.h"
#include "Net/NetIf.h"
#include "Net/Router.h"

namespace Net::IPv4
{
	static void Forward(const Packet& packet, NetIf& dst_if)
	{
		//Clone packet
		Packet* cloned = dst_if.driver.TxAlloc();
		_ASSERT(cloned);
		*cloned = packet;

		//Pop ip hdr and send
		cloned->offset -= sizeof(ipv4_hdr_t);
		dst_if.eth.Send(dst_if, *cloned);
	}
	
	void Receive(NetIf& net_if, Packet& packet)
	{
		_ASSERT(packet.length() >= sizeof(ipv4_hdr_t));

		Deserializer deser(packet.buffer(), packet.length());
		const ipv4_hdr_t ip_hdr =
		{
			.ihl_version = deser.read<uint8_t>(),
			.dscp = deser.read<uint8_t>(),
			.length = deser.read<uint16_t>(),
			.id = deser.read<uint16_t>(),
			.fragment_flags = deser.read<uint16_t>(),
			.ttl = deser.read<uint8_t>(),
			.proto = deser.read<uint8_t>(),
			.checksum = deser.read<uint16_t>(),
			.src = {.addr = deser.read<uint32_t>() },
			.dst = {.addr = deser.read<uint32_t>() },
		};
		packet.offset += sizeof(ipv4_hdr_t);

		//Populate packet values
		packet.src = { ip_hdr.src, 0 };
		packet.dst = { ip_hdr.dst, 0 };

		//Forward packet if needed
		const bool is_broadcast = packet.dst.addr == Net::broadcast;
		const bool is_match = net_if.ipv4.addr == packet.dst.addr;
		const bool is_joined = net_if.ipv4.is_joined(packet.dst.addr);
		const bool is_multicast = Net::is_multicast(packet.dst.addr);
		const bool accept = is_multicast ? is_joined : (is_match || is_broadcast);
		if (is_multicast)
		{
			for (const ipv4_mc_route_t& route : net_if.ipv4.mc_routes)
			{
				if (route.group != packet.dst.addr)
					continue;

				_ASSERT(route.dst);
				Forward(packet, *route.dst);
			}
		}
		else if (!is_match && net_if.ipv4.ip_forwarding)
		{
			//Attempt to forward
			const size_t if_idx = Router::Resolve(packet.dst);
			NetIf& target_if = Router::GetInterface(if_idx);
			if (&target_if != &net_if)
				Forward(packet, target_if);
		}
		
		if (accept)
		{
			switch (ip_hdr.proto)
			{
				case ip_proto_icmp:
					net_if.icmp.Receive(net_if, packet);
					break;

				case ip_proto_udp:
					net_if.udp.Receive(net_if, packet);
					break;

				default:
					_ASSERT(false);
					return;
			}
		}
		else
		{
			//Drop
			packet.release();
		}
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		_ASSERT(packet.offset >= sizeof(ipv4_hdr_t));
		packet.offset -= sizeof(ipv4_hdr_t);

		const ipv4_hdr_t hdr =
		{
			.ihl_version = 0x45,
			.dscp = 0,
			.length = static_cast<uint16_t>(packet.length()),
			.id = 0,
			.fragment_flags = 0,
			.ttl = 64,
			.proto = get_proto(packet.proto),
			.checksum = 0x0000,
			.src = packet.src.addr,
			.dst = packet.dst.addr,
		};

		//TODO(tsharpe): Enable ip checksum
		const uint16_t checksum = Net::checksum(&hdr, sizeof(hdr));

		Serializer ser(packet.buffer(), packet.length());
		ser.write(hdr.ihl_version);
		ser.write(hdr.dscp);
		ser.write(hdr.length);
		ser.write(hdr.id);
		ser.write(hdr.fragment_flags);
		ser.write(hdr.ttl);
		ser.write(hdr.proto);
		ser.write(hdr.checksum);
		ser.write(hdr.src.addr);
		ser.write(hdr.dst.addr);

		return net_if.eth.Send(net_if, packet);
	}
}
