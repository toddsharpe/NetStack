#include "Blade.h"
#include "Net/Socket.h"
#include "Net/Router.h"

Blade::Blade() : upstream("eth0"), peer_a("peer_a"), peer_b("peer_b")
{

}

void Blade::Init()
{
	//Add interfaces
	Net::Router::AddInterface(upstream);
	Net::Router::AddInterface(peer_a);
	Net::Router::AddInterface(peer_b);

	//Initialize upstream
	upstream.Set(1, Net::build_ip(10, 248, 0, 1), Net::subnet_24);
	upstream.ipv4.mc_groups =
	{
		Net::build_ip(224, 0, 0, 251)
	};
	upstream.ipv4.mc_routes =
	{ {
		{ Net::build_ip(224, 0, 0, 251), &peer_a },
		{ Net::build_ip(224, 0, 0, 251), &peer_b }
	} };

	//Initialize peer a
	peer_a.Set(0, Net::build_ip(10, 248, 0, 0), Net::subnet_24);
	Net::Router::AddRoute(peer_a, Net::build_ip(192, 168, 0, 56), Net::subnet_32);
}

void Blade::Dispatch()
{
	PreDispatch();

	PostDispatch();
}

void Blade::WriteUpstream(const void* const buffer, const size_t length)
{
	upstream.dma.rx_write(buffer, length);
}

void Blade::WritePeerA(const void* const buffer, const size_t length)
{
	peer_a.dma.rx_write(buffer, length);
}

void Blade::WritePeerB(const void* const buffer, const size_t length)
{
	peer_b.dma.rx_write(buffer, length);
}

void Blade::VerifyUpstream(const uint8_t* const buffer, const size_t length, const size_t skip)
{
	upstream.dma.tx_complete = true;
	upstream.dma.verify_tx(buffer, length, skip);
}
void Blade::VerifyPeerA(const uint8_t* const buffer, const size_t length, const size_t skip)
{
	peer_a.dma.tx_complete = true;
	peer_a.dma.verify_tx(buffer, length, skip);
}
void Blade::VerifyPeerB(const uint8_t* const buffer, const size_t length, const size_t skip)
{
	peer_b.dma.tx_complete = true;
	peer_b.dma.verify_tx(buffer, length, skip);
}

void Blade::PreDispatch()
{
	upstream.PreDispatch();
	peer_a.PreDispatch();
	peer_b.PreDispatch();
}

void Blade::PostDispatch()
{
	upstream.PostDispatch();
	peer_a.PostDispatch();
	peer_b.PostDispatch();
}
