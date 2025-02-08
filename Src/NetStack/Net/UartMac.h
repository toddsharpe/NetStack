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

		void RxPoll(NetIf& net_if) override;
		bool TxSend(Packet& packet) override;
		void TxFlush() override;

		Packet* RxAlloc() override;
		Packet* TxAlloc() override;

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
