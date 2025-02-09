#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

namespace Win::Eth
{
	void Receive(Net::NetIf& net_if, Net::Packet& packet);
	bool Send(Net::NetIf& net_if, Net::Packet& packet);

	const Net::NetLayer WinEthLayer = { Win::Eth::Receive, Win::Eth::Send };
}
