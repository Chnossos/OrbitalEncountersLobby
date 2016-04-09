#include <OrbitalEncounters/Network/Session.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/CreateRoom.hpp>
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
		{ "CreateRoom", [] (ThreadPool & tp, Session & s, std::string data) {
			tp["App"].push<msg::CreateRoom>(s.shared_from_this(), data);
		}}
	};
}

Session::Session(Id const id, decltype(_socket) && socket)
: _id     { id }
, _socket { std::move(socket) }
{}

Session::~Session()
{
	Log {} << "S[" << _id << "] " << __FUNCTION__ << '\n';
}

void Session::run()
{
	Log {} << "Session [" << _id << "] has started!\n";

	send("WhatDoYouWant");
	recv();
}

void Session::send(Packet const & packet)
{
	// The packet must survive until the completion of the send operation
	auto data = std::make_shared<std::string>(packet.str() + '\0');

	// The session must survive even if it disconnects during the send operation
	auto self = shared_from_this();

	namespace pch = std::placeholders;

	boost::asio::async_write(_socket, boost::asio::buffer(*data),
		std::bind(&Session::onPacketSent, this, self, data, pch::_1, pch::_2)
	);
}

void Session::recv()
{
	std::weak_ptr<Session> const self { shared_from_this() };

	namespace pch = std::placeholders;

	boost::asio::async_read_until(_socket, _buffer, '\0',
		std::bind(&Session::onPacketReceived, this, self, pch::_1)
	);
}

void Session::onPacketSent(Session::Ptr, std::shared_ptr<std::string> packet,
						   boost::system::error_code const & ec, std::size_t)
{
	if (ec) // TODO: better error handling
	{
		Log { std::cerr } << "S[" << _id << "] Error[send]: "
		                  << ec.message() << '\n';
		return;
	}

	if (packet->size() > 256)
		Log {} << "S[" << _id << "] sent: <" << packet->substr(0, 253) << "...>\n";
	else
		Log {} << "S[" << _id << "] sent: <" << *packet << ">\n";
}

void Session::onPacketReceived(Session::WPtr self,
							   boost::system::error_code const & ec)
{
	if (ec) // TODO: better error handling
	{
		if (!self.expired())
			Log { std::cerr } << "S[" << _id << "] "
			                  << "Error(" << ec.value() << "): "
			                  << ec.message() << '\n';
		return;
	}

	// ------------------------------------------------------------------------
	// Extract the packet from the buffer
	// ------------------------------------------------------------------------

	std::string  packet;
	std::istream is { &_buffer };
	std::getline(is, packet, '\0');

	if (packet.size() > 256)
		Log {} << "S[" << _id << "] recv: <" << packet.substr(0, 253) << "...>\n";
	else
		Log {} << "S[" << _id << "] recv: <" << packet << ">\n";

	// ------------------------------------------------------------------------
	// Extract the packet's header and exec the associated handler if it exists
	// ------------------------------------------------------------------------

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