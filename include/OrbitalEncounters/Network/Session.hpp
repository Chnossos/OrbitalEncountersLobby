#pragma once

#include <OrbitalEncounters/Network/Packet.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/streambuf.hpp>
#include <array>
#include <memory>

struct Packet;
class Room;

/// An actual connection with a user.
class Session : public std::enable_shared_from_this<Session>
{
public:
	using Id   = std::uint32_t;
	using Ptr  = std::shared_ptr<Session>;

	using RoomPtr = std::shared_ptr<Room>;

private:
	Id const                     _id;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::ip::udp::socket _udpSocket;
	boost::asio::streambuf       _buffer;
	std::string                  _name;
	std::weak_ptr<Room>          _room;

	// deadline_timer is not movable, so we make it a pointer
	std::unique_ptr<
		boost::asio::deadline_timer> _pingTimer;
	std::time_t                      _lastPongTime;

public:
	/// Constructor.
	Session(Id const id, decltype(_socket) && socket);

	/// Destructor.
	~Session();

public:
	/// Get the ID of the object.
	auto id() const { return _id; }

	/// Get the name of the user.
	auto name() const -> std::string const & { return _name; }

	/// Get a pointer to the room the user is in.
	auto room() { return _room.lock(); }

	/// Return a string containing the remote IPv4.
	auto addr() { return _socket.remote_endpoint().address().to_string(); }

public:
	/// Set the name for this user.
	void setName(std::string const & n) { _name = n; }

	/// Indicate in which room this user currently is.
	void setRoom(RoomPtr m) { _room = m; }

	/// Update the time we received the last ping response from the host.
	void updateLastPongTime() {
		std::time(&_lastPongTime);
	}

public:
	/// Start network dialogs with the user.
	void run();

	/// Cancel all pending network operations.
	void shutdown();

	/// Send a packet to the user, asynchronously.
	void send(Packet const & p);

	/// Start an asynchronous receive.
	void recv();

	/// Try to establish a UDP connection with the user.
	void testUDPConnectivity();

	/// Ping the host every now and then.
	void startAliveCheck();

private:
	void onPacketReceived(Session::Ptr, boost::system::error_code const &);
	void onPacketSent(Session::Ptr, std::shared_ptr<std::string> packet,
					  boost::system::error_code const &, std::size_t);
	void onUDPConnect(boost::system::error_code const & ec);
	bool onError(boost::system::error_code const & ec);
	void onAliveCheck(boost::system::error_code const & ec);

public:
	/// Hides gory packet formatting details.
	friend Packet & operator<<(Packet & p, Session const & s);
};
