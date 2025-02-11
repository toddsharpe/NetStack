#include "Net/SerialNet.h"
#include "Net/IPv4.h"
#include "Net/NetIf.h"
#include "Core/Deserializer.h"
#include "Core/Serializer.h"

namespace Net::SerialNet
{
	static const uint8_t arp(const ipv4_addr_t ip, const bool is_slave)
	{
		if (ip == Net::broadcast || Net::is_multicast(ip))
			return is_slave ? 0 : SerialNet::broadcast;
		return static_cast<uint8_t>(ip.addr);
	}

	static bool Accept(NetIf& net_if, const uint8_t& dst)
	{
		if (SerialNet::broadcast == dst)
			return true;

		if (net_if.serial.addr == dst)
			return true;

		for (uint8_t& filter : net_if.serial.filters)
		{
			if (filter == dst)
				return true;
		}

		return false;
	}
	
	void Receive(NetIf& net_if, Packet& packet)
	{
		if (packet.length() < sizeof(serial_hdr_t))
		{
			net_if.serial.rx_dropped++;
			packet.release();
			return;
		}

		Deserializer deser(packet.buffer(), packet.length());
		const serial_hdr_t hdr =
		{
			.dst = deser.read<uint8_t>(),
			.src = deser.read<uint8_t>(),
			.length = deser.read<uint16_t>(),
			.crc = deser.read<uint16_t>(),
		};
		packet.offset += sizeof(serial_hdr_t);
		
		if (hdr.length != packet.count)
		{
			net_if.serial.rx_dropped++;
			packet.release();
			return;
		}

		if (Accept(net_if, hdr.dst))
		{
			net_if.serial.rx_accepted++;
			
			//Idle frame
			if (packet.count == sizeof(serial_hdr_t))
				return;

			//TODO(tsharpe): Speak when spoken to
			packet.proto = Protocol::Eth;
			net_if.ip.Receive(net_if, packet);
		}
		else
		{
			packet.release();
		}
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		packet.offset -= sizeof(serial_hdr_t);
		
		const serial_hdr_t hdr =
		{
			.dst = arp(packet.dst.addr, net_if.serial.is_slave),
			.src = net_if.serial.addr,
			.length = static_cast<uint16_t>(packet.length()),
			.crc = 0xAABB
		};

		//Write header
		Serializer ser(packet.buffer(), packet.length());
		ser.write(hdr.dst);
		ser.write(hdr.src);
		ser.write(hdr.length);
		ser.write(hdr.crc);

		//Send to driver
		return net_if.driver.TxSend(packet);
	}
}
