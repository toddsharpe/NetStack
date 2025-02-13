#pragma once

#include <cstdint>
#include <stdlib.h>

namespace Net
{
	struct ipv4_addr_t
	{
		union
		{
			uint32_t addr;
			uint8_t bytes[4];
		};

		bool operator==(const ipv4_addr_t& other) const
		{
			return addr == other.addr;
		}

		bool operator!=(const ipv4_addr_t& other) const
		{
			return !(*this == other);
		}
	};

	constexpr ipv4_addr_t build_ip(const uint8_t a0, const uint8_t a1, const uint8_t a2, const uint8_t a3)
	{
		return { .bytes = {a3, a2, a1, a0} };
	}

	constexpr ipv4_addr_t empty = build_ip(0, 0, 0, 0);
	constexpr ipv4_addr_t localhost = build_ip(127, 0, 0, 1);
	constexpr ipv4_addr_t server_ip = build_ip(192, 168, 0, 0);
	constexpr ipv4_addr_t telem_ip = build_ip(239, 9, 9, 9);
	constexpr ipv4_addr_t broadcast = build_ip(0xFF, 0xFF, 0xFF, 0xFF);

	constexpr ipv4_addr_t subnet_24 = build_ip(0xFF, 0xFF, 0xFF, 0);
	constexpr ipv4_addr_t subnet_32 = build_ip(0xFF, 0xFF, 0xFF, 0xFF);

	constexpr bool is_network(const ipv4_addr_t ip_1, const ipv4_addr_t ip_2, const ipv4_addr_t subnet)
	{
		return (ip_1.addr & subnet.addr) == (ip_2.addr & subnet.addr);
	}

	struct endpoint_t
	{
		ipv4_addr_t addr;
		uint16_t port;

		bool operator==(const endpoint_t& other) const
		{
			return addr == other.addr && port == other.port;
		}

		bool operator!=(const endpoint_t& other) const
		{
			return !(*this == other);
		}
	};

	constexpr bool is_multicast(const ipv4_addr_t address)
	{
		const uint8_t high = address.addr >> 24;
		return (high & 0xF0) == 0xE0;
	}

	static constexpr size_t mtu = 1500;
	static constexpr size_t l2_res = 16;
	static constexpr size_t buffer_size = mtu + l2_res;

	uint16_t checksum(const void* const buffer, const size_t length);

	inline uint16_t htons(const uint16_t value)
	{
		return _byteswap_ushort(value);
	}
}
