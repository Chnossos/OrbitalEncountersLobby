#include <OrbitalEncounters/Network/SocketListener.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/SocketAccepted.hpp>
#include <functional>

namespace phs = std::placeholders;
namespace sys = boost::system;

SocketListener::SocketListener(boost::asio::io_service & service)
: _acceptor   { service }
, _socket     { service }
{}

bool SocketListener::listen(Port p)
{
	using socket_base = boost::asio::socket_base;

	sys::error_code ec;

	if (_acceptor.is_open())
		onError(_acceptor.close(ec));

	tcp::endpoint ep { tcp::v4(), _port = p };

	if (_acceptor.open(ep.protocol(), ec)
	 || _acceptor.set_option(socket_base::reuse_address { false }, ec)
	 || _acceptor.bind(ep, ec)
	 || _acceptor.listen(socket_base::max_connections, ec))
	    return !onError(ec);

	_acceptor.async_accept(
		_socket, std::bind(&SocketListener::onAccept, this, phs::_1)
	);

	return _is_running = _acceptor.is_open();
}

void SocketListener::close()
{
	_is_running = false;

	sys::error_code ec;
	_acceptor.close(ec);
}

void SocketListener::onAccept(sys::error_code const & ec)
{
	if (onError(ec))
		return;

	Log {} << "New session incoming\n";

	ServiceLocator::get<ThreadPool>()["App"].push<msg::SocketAccepted>(std::move(_socket));

	_acceptor.async_accept
	(
		_socket, std::bind(&SocketListener::onAccept, this, phs::_1)
	);
}

bool SocketListener::onError(sys::error_code const & ec)
{
	switch (ec.value())
	{
		case sys::errc::success:
			return false;

		case sys::errc::address_in_use:
		case 10048: // Windows
		{
			Log {} << "Error: The port " << _port << " is already in use.\n";
			break;
		}

		case sys::errc::operation_canceled:
		case 995: // Windows
		{
			if (_is_running)
				BOOST_FALLTHROUGH;
			else
				break;
		}

		default:
		{
			Log {} << "SocketListener::onError" << '[' << ec.value() << "]: "
			       << ec.message() << ".\n";
			break;
		}
	}
	return true;
}
