#pragma once

#include <cstdint>
#include <cstddef>
#include <crtdbg.h>

class Deserializer
{
public:
	Deserializer(const uint8_t* const _buffer, const size_t _length) : buffer(_buffer), length(_length), offset()
	{

	}

	template <typename T>
	T read() = delete;

	template <>
	uint8_t read<uint8_t>()
	{
		constexpr size_t size = sizeof(uint8_t);
		_ASSERT(offset <= length - size);

		uint8_t ret = buffer[offset];
		offset++;

		return ret;
	}

	template <>
	uint16_t read<uint16_t>()
	{
		constexpr size_t size = sizeof(uint16_t);
		_ASSERT(offset <= length - size);

		uint16_t temp = {};
		memcpy(&temp, &buffer[offset], size);
		offset += size;
		temp = _byteswap_ushort(temp);

		return temp;
	}

	template <>
	uint32_t read<uint32_t>()
	{
		constexpr size_t size = sizeof(uint32_t);
		_ASSERT(offset <= length - size);

		uint32_t temp = {};
		memcpy(&temp, &buffer[offset], size);
		offset += size;
		temp = _byteswap_ulong(temp);

		return temp;
	}

	template <size_t N>
	void read(uint8_t (&dest)[N])
	{
		_ASSERT(offset <= length - N);

		memcpy(dest, &buffer[offset], N);
		offset += N;
	}

	const void* const peek() const
	{
		return &buffer[offset];
	}

	const size_t remaining() const
	{
		return length - offset;
	}

private:
	const uint8_t* const buffer;
	const size_t length;
	size_t offset;
};
