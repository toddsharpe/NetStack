#include "Net/UartMac.h"
#include "Net/NetIf.h"

namespace Net
{
	UartMac::UartMac(FakeDma& _dma) : rx_pool(), tx_pool(), dma(_dma), tx_queue(), rx_packet(), rx_busy(), tx_busy()
	{

	}

	void UartMac::RxPoll(NetIf& net_if)
	{
		//Check status of HW and call rx callback
		if (dma.rx_received)
		{
			dma.rx_received = false;
			rx_callback(dma.rx_count);

			net_if.eth.Receive(net_if, *rx_packet);
		}

		if (rx_busy)
			return;

		rx_packet = rx_pool.Acquire();
		_ASSERT(rx_packet);
		read_async(*rx_packet);
	}

	bool UartMac::TxSend(Packet& packet)
	{
		if (tx_queue.is_full())
			return false;

		tx_queue.push(&packet);
		return true;
	}

	void UartMac::TxFlush()
	{
		if (dma.tx_complete)
		{
			dma.tx_complete = false;
			tx_callback();
		}

		if (tx_busy || tx_queue.is_empty())
			return;

		Packet* top = tx_queue.peek();
		_ASSERT(top);
		send_async(*top);
	}

	Packet* UartMac::RxAlloc()
	{
		return rx_pool.Acquire();
	}

	Packet* UartMac::TxAlloc()
	{
		return tx_pool.Acquire();
	}

	void UartMac::rx_callback(const size_t size)
	{
		rx_busy = false;
		rx_packet->count = size;
	}

	void UartMac::tx_callback()
	{
		Packet* packet = tx_queue.peek();
		_ASSERT(packet);
		packet->release();

		tx_queue.pop();
		tx_busy = false;
	}

	void UartMac::read_async(Packet& packet)
	{
		rx_busy = true;
		dma.rx_buffer = packet.data;
	}

	void UartMac::send_async(Packet& packet)
	{
		dma.tx_buffer = const_cast<uint8_t*>(packet.data);
		dma.tx_count = uint32_t(packet.count);
	}
}
