#include "Net/Dhcp.h"

namespace Net::Dhcp
{
	//Option entries: code, length, data
	void GetOption(const uint8_t* const options, const size_t options_length, const uint8_t code, void* const dst, const size_t length)
	{
		for (size_t i = 0; i < options_length - 1; i++)
		{
			if (options[i] != code || options[i + 1] != length)
				continue;

			memcpy(dst, &options[i + 2], length);
			return;
		}
	}
}
