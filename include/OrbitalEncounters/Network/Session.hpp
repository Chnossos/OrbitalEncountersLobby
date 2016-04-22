#pragma once

#include <OrbitalEncounters/Network/Packet.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>

class Room;

class Session : public std::enable_shared_from_this<Session>
{
public:
	using Id   = std::uint32_t;
	using Ptr  = std::shared_ptr<Session>;
	using WPtr = std::weak_ptr<Session>;

private:
	Id const                     _id;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::streambuf       _buffer;
	Room *                       _room = nullptr;

public:
	Session(Id const id, decltype(_socket) && socket);
	~Session();

public:
	auto id() const { return _id; }
	auto room() { return _room; }

public:
	void setRoom(Room * m) { _room = m; }

public:
	void run();
	void send(Packet const & p);
	void recv();

private:
	void onPacketReceived(Session::Ptr, boost::system::error_code const &);
	void onPacketSent(Session::Ptr, std::shared_ptr<std::string> packet,
					  boost::system::error_code const &, std::size_t);
	bool onError(boost::system::error_code const & ec);
};