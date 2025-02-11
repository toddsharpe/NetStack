#pragma once

#include "Net/Ethernet.h"
#include "Net/SerialNet.h"
#include "Net/IPv4.h"
#include "Net/Icmp.h"
#include "Net/Udp.h"
#include "Net/Socket.h"

namespace Net
{
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

		IPv4::data_t ipv4;
		SerialNet::data_t serial;
		Ethernet::data_t ethernet;

		std::string name;
	};
}
