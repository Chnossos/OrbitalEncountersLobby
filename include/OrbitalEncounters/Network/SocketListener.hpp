#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

/**
 * Can listen on a specific TCP port to accept incoming connections.
 *
 * @remark     Can only listen to one port at a time.
 */
class SocketListener : public Service
{
	using tcp  = boost::asio::ip::tcp;
	using Port = std::uint16_t;

private:
	tcp::acceptor _acceptor;
	tcp::socket   _socket;
	Port          _port;
	bool          _is_running = false;

public:
	/// Constructor.
	SocketListener(boost::asio::io_service & service);

public:
	/// Start listening on the specified TCP port.
	bool listen(Port p);

	/// Close the acceptor.
	void close();

private:
	void onAccept(boost::system::error_code const & ec);
	bool onError(boost::system::error_code const & ec);
};
