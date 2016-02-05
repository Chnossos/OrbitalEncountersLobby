#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <functional>

namespace msg {

struct SocketAccepted
{
	boost::asio::ip::tcp::socket socket;

	SocketAccepted(decltype(socket) && s)
	: socket { std::move(s) }
	{}
};

} // namespace msg
