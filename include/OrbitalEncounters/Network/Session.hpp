#pragma once

#include <OrbitalEncounters/Network/Packet.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>

struct Packet;
class Room;

class Session : public std::enable_shared_from_this<Session>
{
public:
	using Id   = std::uint32_t;
	using Ptr  = std::shared_ptr<Session>;

private:
	Id const                     _id;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::ip::udp::socket _udpSocket;
	boost::asio::streambuf       _buffer;
	std::string                  _name;
	Room *                       _room;

public:
	Session(Id const id, decltype(_socket) && socket);
	~Session();

public:
	auto id() const { return _id; }
	auto name() const -> std::string const & { return _name; }
	auto room() { return _room; }
	auto addr() { return _socket.remote_endpoint().address().to_string(); }

public:
	void setName(std::string const & n) { _name = n; }
	void setRoom(Room * m) { _room = m; }

public:
	void run();
	void shutdown();
	void send(Packet const & p);
	void recv();
	void testUDPConnectivity();

private:
	void onPacketReceived(Session::Ptr, boost::system::error_code const &);
	void onPacketSent(Session::Ptr, std::shared_ptr<std::string> packet,
					  boost::system::error_code const &, std::size_t);
	void onUDPConnect(boost::system::error_code const & ec);
	bool onError(boost::system::error_code const & ec);

public:
	friend Packet & operator<<(Packet & p, Session const & s);
};