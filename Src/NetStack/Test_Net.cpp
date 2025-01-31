#include "Net/Net.h"
#include <crtdbg.h>

void Test_Net()
{
	/*
	 * Subnet test.
	 */
	{
		const Net::ipv4_addr_t ip_1 = Net::build_ip(10, 248, 0, 0);
		const Net::ipv4_addr_t ip_2 = Net::build_ip(10, 248, 0, 1);
		const Net::ipv4_addr_t subnet = Net::build_ip(255, 255, 255, 0);
		_ASSERT(Net::is_network(ip_1, ip_2, subnet));
	}
	{
		const Net::ipv4_addr_t ip_1 = Net::build_ip(10, 248, 0, 0);
		const Net::ipv4_addr_t ip_2 = Net::build_ip(10, 248, 1, 0);
		const Net::ipv4_addr_t subnet = Net::build_ip(255, 255, 255, 0);
		_ASSERT(Net::is_network(ip_1, ip_2, subnet) == false);
	}
}
