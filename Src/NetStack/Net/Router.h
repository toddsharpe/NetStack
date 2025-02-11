#pragma once

#include "Net/NetTypes.h"
#include "Net/Packet.h"

namespace Net
{
	namespace Router
	{
		size_t Resolve(Net::endpoint_t dst);
		NetIf& GetInterface(const size_t if_idx);
		
		bool AddInterface(NetIf& net_if);
		bool AddRoute(NetIf& net_if, const ipv4_addr_t dst, const ipv4_addr_t subnet_mask);
	}
}
