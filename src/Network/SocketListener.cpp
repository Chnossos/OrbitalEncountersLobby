#include <OrbitalEncounters/Network/SocketListener.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/SocketAccepted.hpp>
#include <functional>

namespace pch = std::placeholders;
namespace sys = boost::system;

SocketListener::SocketListener(boost::asio::io_service & service)
: _acceptor { service }
, _socket   { service }
{}

/**
 * @remark     Calling this method again will cancel the previous operations.
 *
 * @param[in]  p     The TCP port to listen to.
 *
 * @return     @c true if incoming connections are accepted else @c false.
 */
bool SocketListener::listen(Port p)
{
	using socket_base = boost::asio::socket_base;

	sys::error_code ec;

	if (_acceptor.is_open())
		onError(_acceptor.close(ec));

	tcp::endpoint ep { tcp::v4(), _port = p };

	if (_acceptor.open(ep.protocol(), ec)
	 || _acceptor.set_option(socket_base::reuse_address { true }, ec)
	 || _acceptor.bind(ep, ec)
	 || _acceptor.listen(socket_base::max_connections, ec))
		return !onError(ec);

	_acceptor.async_accept(
	    _socket, std::bind(&SocketListener::onAccept, this, pch::_1)
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

	Log {} << __FUNCTION__ << '\n';

	ServiceLocator::get<ThreadPool>()["App"]
	    .push<msg::SocketAccepted>(std::move(_socket));

	_acceptor.async_accept(
	    _socket, std::bind(&SocketListener::onAccept, this, pch::_1)
	);
}

bool SocketListener::onError(sys::error_code const & ec)
{
	switch (ec.default_error_condition().value())
	{
		case sys::errc::success:
			return false;

		case sys::errc::address_in_use:
		{
			Log { std::cerr } << "Error: The port " << _port
			                  << " is already in use.\n";
			break;
		}

		case sys::errc::operation_canceled:
		{
			if (_is_running) // Darn, shit happened...
				BOOST_FALLTHROUGH;
			else // All part of the plan, go on
				break;
		}

		default:
		{
			Log { std::cerr } << "SocketListener::onError" << '['
			                  << ec.default_error_condition().value()
			                  << "]: " << ec.message() << ".\n";
			break;
		}
	}
	return true;
}
