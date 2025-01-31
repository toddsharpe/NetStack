#pragma once

#include "Net/NetTypes.h"
#include "Test/FakeDma.h"
#include "Net/UartMac.h"
#include "Net/NetIf.h"
#include <string>

/*
struct debug_interface_t : Net::NetInterface
{
	debug_interface_t() : uart_inst(dma)
	{
		driver = &uart_inst;
	}

	void RxPoll()
	{
		uart_inst.rx_poll(this);
	}

	FakeDma& dma;
	Net::UartMac::Instance uart_inst;
};
*/

/*
struct EthernetInterface : Net::NetIf
{
	EthernetInterface(const std::string _name) : NetIf(Net::Default, )
	{

	};

	UartMac
};
*/

struct SerialInterface : Net::NetIf
{
	SerialInterface(const std::string& _name) : NetIf(uart_mac, Net::SerialNetLayer, _name), uart_mac(dma)
	{

	}

	void Set(const uint8_t board_id, const Net::ipv4_addr_t board_ip, const Net::ipv4_addr_t subnet)
	{
		config.serial_addr = board_id;
		config.ip.addr = board_ip;
		config.ip.subnet_mask = subnet;
	}

	void PreDispatch()
	{
		uart_mac.RxPoll(*this);
	}

	void PostDispatch()
	{
		uart_mac.TxFlush();
	}

	FakeDma dma;
	Net::UartMac uart_mac;
};
