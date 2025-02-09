#pragma once

#include "WinMac.h"
#include "WinEth.h"

struct WinNetIf : Net::NetIf
{
	WinNetIf(const std::string& _name) : NetIf(win_mac, Win::Eth::WinEthLayer, _name), win_mac()
	{

	}

	void Init()
	{
		win_mac.Init();
	}

	void Set(const Net::ipv4_addr_t board_ip, const Net::ipv4_addr_t subnet)
	{
		ipv4.addr = board_ip;
		ipv4.subnet_mask = subnet;
	}

	void PreDispatch()
	{
		win_mac.RxPoll(*this);
	}

	void PostDispatch()
	{
		win_mac.TxFlush();
	}

	WinMac win_mac;
};
