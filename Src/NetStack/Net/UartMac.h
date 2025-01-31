#pragma once

#include "Net/NetTypes.h"
#include "Net/Packet.h"
#include "Core/CircularBuffer.h"
#include "Test/FakeDma.h"
#include <array>

namespace Net
{
	class UartMac : public Net::NetDriver
	{
	public:
		UartMac(FakeDma& _dma);

		virtual void RxPoll(NetIf& net_if);
		virtual bool TxSend(Packet& packet);
		virtual void TxFlush();

		virtual Packet* RxAlloc();
		virtual Packet* TxAlloc();

	private:
		void rx_callback(const size_t size);
		void tx_callback();

		void read_async(Packet& packet);
		void send_async(Packet& packet);

		PacketPool<2> rx_pool;
		PacketPool<4> tx_pool;
		FakeDma& dma;
		CircularBuffer<Packet*, 8> tx_queue;
		Packet* rx_packet;
		bool rx_busy;
		bool tx_busy;
	};
}
