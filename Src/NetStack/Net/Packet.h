#pragma once

#include "Net/Net.h"
#include <cstdint>
#include <cstddef>
#include <array>
#include <atomic>

namespace Net
{
	enum class Protocol
	{
		None,
		Eth,
		Ipv4,
		Udp,
		Icmp,
		EchoReply,
		EchoRequest
	};
	
	struct Packet
	{
		Packet() : data(), count(), offset(), src(), dst(), in_use(), proto()
		{

		}
		
		uint8_t* buffer()
		{
			return &data[offset];
		}

		size_t length() const
		{
			if (count == 0)
				return sizeof(data) - offset;
			else
				return count - offset;
		}

		void release()
		{
			memset(data, 0, sizeof(data));
			count = 0;
			offset = 0;
			src = {};
			dst = {};
			proto = Protocol::None;
			in_use = false;
		}

		//Packet data
		uint8_t data[buffer_size];
		size_t count;
		size_t offset;
		
		//Layer specific data
		endpoint_t src;
		endpoint_t dst;
		Protocol proto;

		bool in_use;
	};

	template <size_t N>
	class PacketPool
	{
	public:
		PacketPool() : entries()
		{

		}

		Packet* Acquire()
		{
			for (size_t i = 0; i < entries.size(); i++)
			{
				/*
				if (in_use[i])
					continue;

				in_use[i] = true;
				return &entries[i];
				*/

				if (entries[i].in_use)
					continue;

				entries[i].in_use = true;
				return &entries[i];
			}

			return nullptr;
		}

		/*
		void Release(const Packet* packet)
		{
			const size_t index = ((uintptr_t)packet / (uintptr_t)entries.data()) / sizeof(Packet);

			_ASSERT(in_use[index]);
			in_use[index] = false;
		}
		*/

	private:
		std::array<Packet, N> entries;
		//std::array<bool, N> in_use;
	};

	template <size_t N>
	inline void CopyTo(Packet& dst, const uint8_t(&src)[N])
	{
		memcpy(dst.data, src, N);
		dst.count = N;
	}

	inline void CopyTo(Packet& dst, const uint8_t* const src, const size_t length)
	{
		memcpy(dst.data, src, length);
		dst.count = length;
	}
}
