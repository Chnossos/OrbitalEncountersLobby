#pragma once

#include <OrbitalEncounters/Network/Packet.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>

class Session : public std::enable_shared_from_this<Session>
{
public:
	using Id  = std::uint32_t;
	using Ptr = std::shared_ptr<Session>;

	Id const id;

private:
	boost::asio::ip::tcp::socket _socket;
	boost::asio::streambuf       _buffer;

public:
	Session(Id id, decltype(_socket) && socket);
	~Session();

public:
	void run();

public:
	void send(Packet const & p);
	void recv();

private:
	void onPacketReceived(std::weak_ptr<Session>, boost::system::error_code const &);
};