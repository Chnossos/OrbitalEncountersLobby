#include <OrbitalEncounters/Session.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <istream>
#include <string>

Session::Session(Id id, decltype(_socket) && socket)
: id      { id }
, _socket { std::move(socket) }
{}

Session::~Session()
{
	Log {} << __FUNCTION__ << '\n';
}

void Session::run()
{
	Log {} << "Session [" << id << "] has started!\n";
	send("HG");
	recv();
}

void Session::send(Packet const & packet)
{
	std::string data = packet.str();

	if (data.size() > 256)
		Log {} << '<' << data.substr(0, 253) << "...>\n";
	else
		Log {} << '<' << data << ">\n";

	auto self { shared_from_this() };

	boost::asio::async_write(
		_socket, boost::asio::buffer(data += '\0'),
		[self] (boost::system::error_code const &, std::size_t) {}
	);
}

void Session::recv()
{
	namespace pch = std::placeholders;

	std::weak_ptr<Session> const self { shared_from_this() };

	boost::asio::async_read_until(
		_socket, _buffer, '\0',
		std::bind(&Session::onPacketReceived, this, self, pch::_1)
	);
}

void Session::onPacketReceived(std::weak_ptr<Session>            ptr,
							   boost::system::error_code const & ec)
{
	if (ec)
	{
		if (!ptr.expired())
			Log { std::cerr } << "S[" << id
			                  << "] Error(" << ec.value()
			                  << "): " << ec.message()
			                  << '\n';
		return;
	}

	std::string  packet;
	std::istream is { &_buffer };
	std::getline(is, packet, '\0');

	if (packet.size() > 256)
		Log {} << "S[" << id << "] recv: <" << packet.substr(0, 253) << "...>\n";
	else
		Log {} << "S[" << id << "] recv: <" << packet << ">\n";

	recv();
}