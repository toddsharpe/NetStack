#pragma once

#include "Net/NetTypes.h"
#include "Core/CircularBuffer.h"
#include "Net/Packet.h"
#include "winsock2.h"
#include "ws2tcpip.h" //IP_HDRINCL is here
#include "Net/NetIf.h"

#pragma comment(lib, "Ws2_32.lib")

class WinMac : public Net::NetDriver
{
public:
	WinMac() : rx_pool(), tx_pool(), tx_queue(), s()
	{

	}

	void Init()
	{
		WSADATA wsock = {};
		_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsock) == 0);
		s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
		_ASSERT(s != SOCKET_ERROR);

		//Unclear if needed
		int optval = 1;
		const int ret = setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof(optval));
		_ASSERT(ret != SOCKET_ERROR);
	}

	void RxPoll(Net::NetIf& net_if) override
	{
		Net::Packet* rx_packet = rx_pool.Acquire();
		_ASSERT(rx_packet);

		SOCKADDR_IN from = {};
		int bytes_read = 0;
		const int ret = recvfrom(s, reinterpret_cast<char*>(rx_packet->data), sizeof(rx_packet->data), 0, (sockaddr*)&from, &bytes_read);
		if (ret == SOCKET_ERROR)
			return;
		_ASSERT(ret != SOCKET_ERROR);

		rx_packet->count = bytes_read;
		net_if.eth.Receive(net_if, *rx_packet);
	}

	bool TxSend(Net::Packet& packet) override
	{
		if (tx_queue.is_full())
			return false;

		tx_queue.push(&packet);
		return true;
	}
	
	void TxFlush() override
	{
		if (tx_queue.is_empty())
			return;

		//Get top of queue
		Net::Packet* top = tx_queue.peek();
		_ASSERT(top);

		//Build dest, even in Raw its needed for windows to know which interface to send to
		SOCKADDR_IN dest =
		{
			.sin_family = AF_INET,
			.sin_port = top->dst.port,
		};
		dest.sin_addr.S_un.S_addr = top->dst.addr.addr;

		const int ret = sendto(s, reinterpret_cast<char*>(top->buffer()), top->length(), 0, (SOCKADDR*)&dest, sizeof(dest));
		_ASSERT(ret != SOCKET_ERROR);
	}

	Net::Packet* RxAlloc() override
	{
		return rx_pool.Acquire();
	}

	Net::Packet* TxAlloc() override
	{
		return tx_pool.Acquire();
	}

private:
	Net::PacketPool<2> rx_pool;
	Net::PacketPool<4> tx_pool;
	CircularBuffer<Net::Packet*, 8> tx_queue;

	SOCKET s;
};
