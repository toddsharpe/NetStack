#pragma once

#include "Net/Packet.h"
#include "Core/Memory.h"

#include <cstdint>

class FakeDma
{
public:
	FakeDma() : rx_buffer(), tx_buffer(), rx_count(), tx_count(), rx_received(), tx_complete()
	{

	}

	void rx_write(const void* const buffer, const size_t length)
	{
		memcpy(rx_buffer, buffer, length);
		rx_count = static_cast<uint32_t>(length);
		rx_received = true;
	}

	void verify_tx(const uint8_t* const buffer, const size_t length, const size_t skip)
	{
		_ASSERT(tx_complete);
		_ASSERT(tx_count == length);
		_ASSERT(mem_cmp(&tx_buffer[skip], &buffer[skip], length - skip));
	}

	void* rx_buffer;
	uint8_t* tx_buffer;

	uint32_t rx_count;
	uint32_t tx_count;

	bool rx_received;
	bool tx_complete;
};
