#include "WinEth.h"
#include "Net/NetIf.h"

namespace Win::Eth
{
	void Receive(Net::NetIf& net_if, Net::Packet& packet)
	{
		net_if.ip.Receive(net_if, packet);
	}

	bool Send(Net::NetIf& net_if, Net::Packet& packet)
	{
		return net_if.driver.TxSend(packet);
	}
}
