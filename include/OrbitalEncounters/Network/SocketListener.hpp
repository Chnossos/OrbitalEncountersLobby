#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

class SocketListener
{
	using tcp  = boost::asio::ip::tcp;
	using Port = std::uint16_t;

private:
	tcp::acceptor _acceptor;
	tcp::socket   _socket;
	Port          _port;
	bool          _is_running = false;

public:
	SocketListener(boost::asio::io_service & service);

public:
	bool listen(Port p);
	void close();

private:
	void onAccept(boost::system::error_code const & ec);
	bool onError(boost::system::error_code const & ec);
};
