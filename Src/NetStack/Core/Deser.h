#pragma once

#include <cstdint>
#include <cstddef>
#include <crtdbg.h>

class Deser
{
public:
	Deser(const uint8_t* const _buffer, const size_t _length) : buffer(_buffer), length(_length), offset()
	{

	}

	template <typename T>
	T read()
	{
		constexpr size_t size = sizeof(T);
		static_assert(size >= 0 && size < 8);
		
		_ASSERT(offset <= length - size);
			
		uint8_t temp[size] = {};
		memcpy(temp, &buffer[offset], size);
		offset += size;

		//Endian swap
		for (size_t i = 0; i < size / 2; i++)
		{
			uint8_t swap = temp[i];
			temp[i] = temp[size - i - 1];
			temp[size - i - 1] = swap;
		}

		return *(T*)temp;
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

class Ser
{
public:
	Ser(uint8_t* const _buffer, const size_t _length) : buffer(_buffer), length(_length), offset()
	{

	}

	template <typename T>
	void write(T value)
	{
		_ASSERT(offset <= length - sizeof(T));

		uint8_t temp[sizeof(T)] = {};
		memcpy(temp, &value, sizeof(T));

		//Endian swap
		for (size_t i = 0; i < sizeof(T) / 2; i++)
		{
			uint8_t swap = temp[i];
			temp[i] = temp[sizeof(T) - i - 1];
			temp[sizeof(T) - i - 1] = swap;
		}

		memcpy(&buffer[offset], temp, sizeof(T));
		offset += sizeof(T);
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
