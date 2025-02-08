#include "Net/Ethernet.h"
#include "Net/IPv4.h"
#include "Net/NetIf.h"
#include "Core/Deserializer.h"

namespace Net::Ethernet
{
	static bool Accept(NetIf& net_if, const eth_mac_t& dst)
	{
		if (Ethernet::broadcast == dst)
			return true;
		
		if (net_if.ethernet.addr == dst)
			return true;

		for (eth_mac_t& filter : net_if.ethernet.filters)
		{
			if (filter == dst)
				return true;
		}

		return false;
	}
	
	void Receive(NetIf& net_if, Packet& packet)
	{
		Deserializer deser(packet.buffer(), packet.length());
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

		if (Accept(net_if, hdr.dst))
		{
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
		else
		{
			packet.release();
		}
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		return false;
	}
}
