#pragma once

#include "Net/Packet.h"
#include <string>

namespace Net
{
	struct NetIf;
	
	class NetDriver
	{
	public:
		/*
		 * Issue IO.
		 */
		virtual void RxPoll(NetIf& net_if) = 0;
		virtual bool TxSend(Packet& packet) = 0;
		virtual void TxFlush() = 0;

		/*
		 * Packet.
		 */
		virtual Packet* RxAlloc() = 0;
		virtual Packet* TxAlloc() = 0;
	};

	typedef void (*rx_t)(NetIf& interface, Packet& packet);
	typedef bool (*tx_t)(NetIf& interface, Packet& packet);

	struct NetLayer
	{
		NetLayer() : Receive(), Send(), HeaderSize()
		{

		}
		
		NetLayer(const rx_t _rx, const tx_t _tx, const size_t _header_size = 0) : Receive(_rx), Send(_tx), HeaderSize(_header_size)
		{

		}

		const rx_t Receive;
		const tx_t Send;
		const size_t HeaderSize;
	};
}
