#include "Net/Router.h"
#include "Net/NetIf.h"

namespace
{
	struct if_entry_t
	{
		Net::NetIf* net_if;
		bool in_use;
	};

	struct uc_route_entry_t
	{
		Net::NetIf* dst_if;
		Net::ipv4_addr_t addr;
		Net::ipv4_addr_t subnet_mask;
		bool in_use;
	};

	std::array<if_entry_t, 4> if_entries;
	std::array<uc_route_entry_t, 4> uc_routes;
}

namespace Net::Router
{
	size_t Resolve(Net::endpoint_t dst)
	{
		//Route based on interface subnet/multicast
		for (size_t i = 0; i < if_entries.size(); i++)
		{
			if (!if_entries[i].in_use)
				continue;

			const bool is_network = Net::is_network(dst.addr, if_entries[i].net_if->ipv4.addr, if_entries[i].net_if->ipv4.subnet_mask);
			const bool is_multicast = Net::is_multicast(dst.addr);
			const bool is_joined = if_entries[i].net_if->ipv4.is_joined(dst.addr);
			const bool accept = is_multicast ? is_joined : is_network;
			if (accept)
				return i;
		}

		//Route based on manual routes
		for (uc_route_entry_t& route : uc_routes)
		{
			if (!route.in_use)
				continue;

			const bool is_network = Net::is_network(dst.addr, route.addr, route.subnet_mask);
			if (!is_network)
				continue;

			//Get interface idx for route
			for (size_t i = 0; i < if_entries.size(); i++)
			{
				if (route.dst_if == if_entries[i].net_if)
					return i;
			}
			_ASSERT(false);
		}

		//Nothing matched, return default interface
		return 0;
	}

	NetIf& Net::Router::GetInterface(const size_t if_idx)
	{
		_ASSERT(if_idx < if_entries.size());
		_ASSERT(if_entries[if_idx].in_use);
		_ASSERT(if_entries[if_idx].net_if);

		return *if_entries[if_idx].net_if;
	}
	
	bool AddInterface(NetIf& net_if)
	{
		for (size_t i = 0; i < if_entries.size(); i++)
		{
			if (if_entries[i].in_use)
				continue;

			if_entries[i].net_if = &net_if;
			if_entries[i].in_use = true;

			return true;
		}

		return false;
	}

	bool AddRoute(NetIf& net_if, const ipv4_addr_t addr, const ipv4_addr_t subnet_mask)
	{
		for (uc_route_entry_t& route : uc_routes)
		{
			if (route.in_use)
				continue;

			route.dst_if = &net_if;
			route.addr = addr;
			route.subnet_mask = subnet_mask;
			route.in_use = true;

			return true;
		}
		return false;
	}
}
