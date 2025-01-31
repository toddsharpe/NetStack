#include "Net/Ethernet.h"
#include "Net/IPv4.h"
#include "Net/NetIf.h"
#include "Core/Deser.h"

namespace Net::Ethernet
{
	void Receive(NetIf& net_if, Packet& packet)
	{
		Deser deser(packet.buffer(), packet.length());
		eth_mac_t dst = {};
		deser.read(dst.bytes);
		eth_mac_t src = {};
		deser.read(src.bytes);

		const eth_hdr_t hdr =
		{
			.dst = dst,
			.src = src,
			.type = deser.read<uint16_t>(),
		};
		packet.offset += sizeof(eth_hdr_t);

		switch (hdr.type)
		{
			case eth_type_ipv4:
				net_if.ip.Receive(net_if, packet);
				break;

			case eth_type_arp:
				_ASSERT(false);
				break;

			default:
				_ASSERT(false);
		}
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		return false;
	}
}
