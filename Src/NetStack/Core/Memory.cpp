#include "Core/Memory.h"
#include <string.h>
#include <stdio.h>
#include <cstdint>

bool mem_cmp(const void* const buffer1, const void* const buffer2, const size_t length)
{
	const bool compare = memcmp(buffer1, buffer2, length) == 0;
	if (compare)
		return true;

	//show byte by byte
	printf("Mem compare failed:\n");
	printf("-------------------\n");
	printf("Buf1       Buf2    \n");
	for (size_t i = 0; i < length; i++)
	{
		const uint8_t left = reinterpret_cast<const uint8_t*>(buffer1)[i];
		const uint8_t right = reinterpret_cast<const uint8_t*>(buffer2)[i];
		if (left == right)
			continue;

		printf("0x%lX:    0x%02X, 0x%02X\n", i, left, right);
	}

	return false;
}
