#include "Net/Net.h"
#include "Net/NetTypes.h"
#include "Net/IPv4.h"
#include "Net/Udp.h"
#include "Net/Icmp.h"
#include "Net/Socket.h"

namespace Net
{
	// From: https://tools.ietf.org/html/rfc1071
	uint16_t checksum(const void* const buffer, const size_t length)
	{
		uint32_t sum = 0;

		const uint16_t* ptr = reinterpret_cast<const uint16_t*>(buffer);
		size_t count = length;

		while (count > 1) {
			/*  This is the inner loop */
			sum += *ptr++;
			count -= 2;
		}

		/*  Add left-over byte, if any */
		if (count > 0)
			sum += *(uint8_t*)ptr;

		/*  Fold 32-bit sum to 16 bits */
		while (sum >> 16)
			sum = (sum & 0xffff) + (sum >> 16);

		return htons(~sum);
	}
}
