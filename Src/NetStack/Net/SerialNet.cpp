#include "Net/SerialNet.h"
#include "Net/IPv4.h"
#include "Net/NetIf.h"
#include "Core/Deser.h"

namespace Net::SerialNet
{
	namespace
	{
		constexpr uint8_t arp(const ipv4_addr_t ip)
		{
			if (Net::is_multicast(ip))
				return SerialNet::serial_broadcast;
			return static_cast<uint8_t>(ip.addr);
		}
	}
	
	void Receive(NetIf& net_if, Packet& packet)
	{
		_ASSERT(packet.length() >= sizeof(serial_hdr_t));

		Deser deser(packet.buffer(), packet.length());
		const serial_hdr_t hdr =
		{
			.dst = deser.read<uint8_t>(),
			.src = deser.read<uint8_t>(),
			.length = deser.read<uint16_t>(),
			.crc = deser.read<uint16_t>(),
		};
		packet.offset += sizeof(serial_hdr_t);
		_ASSERT(hdr.length == packet.count);

		if (hdr.dst == serial_broadcast)
		{
			//rebroadcast to all
			_ASSERT(false);
		}
		else if (hdr.dst == net_if.config.serial_addr)
		{
			//Idle frame
			if (packet.count == sizeof(serial_hdr_t))
				return;

			//TODO(tsharpe): Speak when spoken to
			net_if.ip.Receive(net_if, packet);
		}
		else
		{
			//net_if.driver->RxFree(packet);
			packet.release();
		}
	}

	bool Send(NetIf& net_if, Packet& packet)
	{
		packet.offset -= sizeof(serial_hdr_t);
		
		const serial_hdr_t hdr =
		{
			.dst = arp(packet.dst.addr),
			.src = net_if.config.serial_addr,
			.length = static_cast<uint16_t>(packet.length()),
			.crc = 0xAABB
		};

		//Write header
		Ser ser(packet.buffer(), packet.length());
		ser.write(hdr.dst);
		ser.write(hdr.src);
		ser.write(hdr.length);
		ser.write(hdr.crc);

		//Send to driver
		return net_if.driver.TxSend(packet);
	}
}
