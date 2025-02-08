#pragma once

#include <cstdint>
#include <cstddef>
#include <crtdbg.h>

class Serializer
{
public:
	Serializer(uint8_t* const _buffer, const size_t _length) : buffer(_buffer), length(_length), offset()
	{

	}

	template <typename T>
	void write(const T value) = delete;

	template <>
	void write<uint8_t>(const uint8_t value)
	{
		constexpr size_t size = sizeof(uint8_t);
		_ASSERT(offset <= length - size);

		buffer[offset] = value;
		offset += size;
	}

	template <>
	void write<uint16_t>(const uint16_t value)
	{
		constexpr size_t size = sizeof(uint16_t);
		_ASSERT(offset <= length - size);

		uint16_t* dst = reinterpret_cast<uint16_t*>(&buffer[offset]);
		*dst = _byteswap_ushort(value);
		offset += size;
	}

	template <>
	void write<uint32_t>(const uint32_t value)
	{
		constexpr size_t size = sizeof(uint32_t);
		_ASSERT(offset <= length - size);

		uint32_t* dst = reinterpret_cast<uint32_t*>(&buffer[offset]);
		*dst = _byteswap_ulong(value);
		offset += size;
	}

	void write_from(const void* const from, const size_t bytes)
	{
		_ASSERT(offset <= length - bytes);
		memcpy(&buffer[offset], from, bytes);
		offset += bytes;
	}

	const size_t bytes() const
	{
		return offset;
	}

private:
	uint8_t* const buffer;
	const size_t length;
	size_t offset;
};
