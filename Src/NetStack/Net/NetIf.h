#pragma once

#include "Net/Ethernet.h"
#include "Net/SerialNet.h"
#include "Net/IPv4.h"
#include "Net/Icmp.h"
#include "Net/Udp.h"
#include "Net/Socket.h"

namespace Net
{
	struct ipv4_data_t
	{
		ipv4_data_t() : mc_groups(), mc_routes(), addr(), subnet_mask(Net::subnet_32), ip_forwarding(true)
		{

		}

		bool is_joined(const ipv4_addr_t mc_group) const
		{
			for (const ipv4_addr_t& addr : mc_groups)
			{
				if (addr == mc_group)
					return true;
			}
			return false;
		}
		
		std::array<ipv4_addr_t, 4> mc_groups;
		std::array<IPv4::ipv4_mc_route_t, 4> mc_routes;
		ipv4_addr_t addr;
		ipv4_addr_t subnet_mask;
		bool ip_forwarding;
	};
	
	struct NetBindings
	{
		NetLayer ip;
		NetLayer udp;
		NetLayer icmp;
		NetLayer socket;
	};

	struct NetIf
	{
		NetIf(NetDriver& _driver, const NetLayer& _eth, const std::string& _name)
			: driver(_driver)
			, eth(_eth)
			, ip(IPv4Layer)
			, udp(UdpLayer)
			, icmp(IcmpLayer)
			, socket(SocketLayer)
			, ipv4()
			, serial()
			, ethernet()
			, name(_name)
		{

		}

		NetIf(NetDriver& _driver, const NetLayer& _eth, const std::string& _name, const NetBindings& bindings)
			: driver(_driver)
			, eth(_eth)
			, ip(bindings.ip)
			, udp(bindings.udp)
			, icmp(bindings.icmp)
			, socket(bindings.socket)
			, ipv4()
			, serial()
			, ethernet()
			, name(_name)
		{

		}

		NetDriver& driver;
		const NetLayer& eth;
		const NetLayer& ip;
		const NetLayer& udp;
		const NetLayer& icmp;
		const NetLayer& socket;

		ipv4_data_t ipv4;
		SerialNet::config_t serial;
		Ethernet::config_t ethernet;

		std::string name;
	};
}
