#include <OrbitalEncounters/Network/Session.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Messages/ConnectivityTestDone.hpp>
#include <OrbitalEncounters/Messages/CreateRoom.hpp>
#include <OrbitalEncounters/Messages/JoinRoom.hpp>
#include <OrbitalEncounters/Messages/PlayerLeaving.hpp>
#include <OrbitalEncounters/Messages/RoomListRequested.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <functional>
#include <istream>
#include <string>
#include <unordered_map>

namespace
{
	using Handler = std::function<void(ThreadPool &, Session &, std::string)>;

	std::unordered_map<std::string, Handler> const handlers =
	{
		{ pkt::CreateRoom, [] (ThreadPool & tp, Session & s, std::string data) {
			tp["App"].push<msg::CreateRoom>(s.shared_from_this(), data);
		}},
		{ pkt::JoinRoom, [] (ThreadPool & tp, Session & s, std::string data) {
			tp["App"].push<msg::JoinRoom>(s.shared_from_this(), data);
		}},
		{ pkt::LeaveRoom, [] (ThreadPool & tp, Session & s, std::string) {
			tp["App"].push<msg::PlayerLeaving>(s.shared_from_this());
		}},
		{ pkt::ListRooms, [] (ThreadPool & tp, Session & s, std::string) {
			tp["App"].push<msg::RoomListRequested>(s.shared_from_this());
		}},
		{ pkt::MyNameIs, [] (ThreadPool &, Session & s, std::string name) {
			s.setName(name);
			s.send(Packet { pkt::YourIPIs } << '|' << s.addr());
		}}
	};
}

Session::Session(Id const id, decltype(_socket) && socket)
: _id        { id }
, _socket    { std::move(socket) }
, _udpSocket { _socket.get_io_service() }
, _pingTimer { _socket.get_io_service() }
, _room      { nullptr }
{}

Session::~Session()
{
	Log {} << "S[" << _id << "] " << __FUNCTION__ << '\n';
}

void Session::run()
{
	Log {} << "Session [" << _id << "] has started!\n";

	send(pkt::WhoAreYou);
	recv();
}

void Session::shutdown()
{
	_socket.cancel();
}

void Session::send(Packet const & packet)
{
	// The packet must survive until the completion of the send operation
	auto data = std::make_shared<std::string>(packet.str() + '\n');

	// The session must survive even if it disconnects during the send operation
	auto self = shared_from_this();

	namespace pch = std::placeholders;

	boost::asio::async_write(_socket, boost::asio::buffer(*data),
		std::bind(&Session::onPacketSent, this, self, data, pch::_1, pch::_2)
	);
}

void Session::recv()
{
	namespace pch = std::placeholders;

	boost::asio::async_read_until(_socket, _buffer, '\0',
		std::bind(&Session::onPacketReceived, this, shared_from_this(), pch::_1)
	);
}

void Session::testUDPConnectivity()
{
	boost::asio::ip::udp::endpoint endpoint { _socket.remote_endpoint().address(), 8000 };
	_udpSocket.async_connect(
		endpoint, std::bind(&Session::onUDPConnect, this, std::placeholders::_1)
	);
}

void Session::onPacketSent(Session::Ptr, std::shared_ptr<std::string> packet,
						   boost::system::error_code const & ec, std::size_t)
{
	if (onError(ec))
		return;

	packet->pop_back(); // Removes leading '\0' packet termination

	if (packet->size() > 256)
		Log {} << "S[" << _id << "] sent: <" << packet->substr(0, 253) << "...>\n";
	else
		Log {} << "S[" << _id << "] sent: <" << *packet << ">\n";
}

void Session::onPacketReceived(Session::Ptr, boost::system::error_code const & ec)
{
	if (onError(ec))
		return;

	// -------------------------------------------------------------------------
	// Extract the packet from the buffer
	// -------------------------------------------------------------------------

	std::string  packet;
	std::istream is { &_buffer };
	std::getline(is, packet, '\0');

	if (packet.size() > 256)
		Log {} << "S[" << _id << "] recv: <" << packet.substr(0, 253) << "...>\n";
	else
		Log {} << "S[" << _id << "] recv: <" << packet << ">\n";

	// -------------------------------------------------------------------------
	// Extract the packet's header and exec. the associated handler if it exists
	// -------------------------------------------------------------------------

	std::string header, data;

	auto pos = packet.find('|');
	if (pos == std::string::npos)
		header = std::move(packet);
	else
	{
		// Only separate the header, data will be handled later as needed
		header = packet.substr(0, pos);
		data   = packet.substr(pos + 1);
	}

	auto it = handlers.find(header);
	if (it != handlers.end())
		it->second(ServiceLocator::get<ThreadPool>(), *this, std::move(data));
	else
		Log { std::cerr } << "S[" << _id << "] "
		                  << "Packet <" << header << "> is undefined!\n";

	recv();
}

void Session::onUDPConnect(boost::system::error_code const & ec)
{
	bool errorOccured = onError(ec);
	send(Packet { pkt::ConnectivityTestDone } << '|' << std::boolalpha << !errorOccured);

	/*if (!errorOccured)
		_udpSocket.close();*/

	_udpSocket.async_receive(boost::asio::buffer(_udpBuffer), std::bind(&Session::onUDPReceived, this, std::placeholders::_1));

	ServiceLocator::get<ThreadPool>()["App"].push<msg::ConnectivityTestDone>(
		shared_from_this(), _room->id(), !errorOccured);
}

void Session::onUDPReceived(boost::system::error_code const & ec)
{
	Log {} << __FUNCTION__ << ':' << ec.message() << '\n';
}

bool Session::onError(boost::system::error_code const & ec)
{
	namespace errc = boost::system::errc;

	switch (ec.default_error_condition().value())
	{
		case errc::success:
			return false;

		case errc::no_such_file_or_directory:
		case errc::connection_reset:
		{
			Log {} << "S[" << _id << "] Disconnected\n";
			ServiceLocator::get<ThreadPool>()["App"].push<msg::SessionDisconnected>(
				shared_from_this()
			);
			break;
		}

		case errc::operation_canceled:
			break;

		default:
		{
			Log {} << "S[" << _id << "] ERROR " << ec.value()
			       << '(' << ec.default_error_condition().value() << ')'
			       << ": " << ec.message() << '\n';
			break;
		}
	}
	return true;
}

Packet & operator<<(Packet & pkt, Session const & s)
{
	return pkt << s._id << ':' << s._name;
}