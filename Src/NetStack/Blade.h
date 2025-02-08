#pragma once

#include "Board.h"

class Blade
{
public:
	Blade();
	
	void Init();
	void Dispatch();

	void WriteUpstream(const void* const buffer, const size_t length);
	void WritePeerA(const void* const buffer, const size_t length);
	void WritePeerB(const void* const buffer, const size_t length);

	void VerifyUpstream(const uint8_t* const buffer, const size_t length, const size_t skip);
	void VerifyPeerA(const uint8_t* const buffer, const size_t length, const size_t skip);
	void VerifyPeerB(const uint8_t* const buffer, const size_t length, const size_t skip);

	void PreDispatch();
	void PostDispatch();

	SerialInterface upstream;
	SerialInterface peer_a;
	SerialInterface peer_b;
};
