#pragma once

#include "Net/Packet.h"
#include "Net/NetTypes.h"

struct NullMac : public Net::NetDriver
{
	void RxPoll(Net::NetIf& net_if)
	{

	}

	bool TxSend(Net::Packet& packet)
	{
		return true;
	}

	void TxFlush()
	{

	}

	Net::Packet* RxAlloc()
	{
		return nullptr;
	}

	Net::Packet* TxAlloc()
	{
		return nullptr;
	}
};
