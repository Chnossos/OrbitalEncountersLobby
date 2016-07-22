#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packet.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>
#include <algorithm>
#include <iterator>

Room::Room(Id const id, Session::Ptr owner)
: _id        { id }
, _sessions  { owner }
, _pingTimer { std::make_unique<boost::asio::deadline_timer>(ServiceLocator::get<ThreadPool>()["App"].service) }
{}

Room::~Room()
{
	Log {} << "R[" << _id << "] " << __FUNCTION__ << '\n';

	_pingTimer->cancel();
}

void Room::addSession(Session::Ptr & session)
{
	_sessions.push_back(session);
	session->setRoom(this);
}

void Room::removeSession(Session::Ptr & s)
{
	// Is this session really in this room ?
	auto it = std::find(_sessions.cbegin(), _sessions.cend(), s);
	if (it == _sessions.cend()) return;

	if (s == _sessions.front()) // Host is leaving, disband
	{
		for (auto & s : _sessions)
			s->setRoom(nullptr);

		_sessions.clear();
		ServiceLocator::get<ThreadPool>()["App"].push<msg::EmptyRoom>(_id);
	}
	else
	{
		s->setRoom(nullptr);
		_sessions.remove(s);
	}
}

void Room::startAliveCheck()
{
	_pingTimer->expires_from_now(boost::posix_time::seconds(10));
	_pingTimer->async_wait(std::bind(&Room::onAliveCheck, this, std::placeholders::_1));
	updateLastPongTime();
}

void Room::onAliveCheck(boost::system::error_code const & ec)
{
	if (ec == boost::asio::error::operation_aborted)
		return;

	auto const interval = std::time(nullptr) - _lastPongTime;

	if (ec) {
		Log { std::cerr } << "Room[" << _id << "] something went wrong with the ping\n";
	}
	else if (interval > 10)
	{
		Log { std::cerr } << "R[" << _id << "] failed to respond to ping in time\n";
		ServiceLocator::get<ThreadPool>()["App"].push<msg::SessionDisconnected>(_sessions.front());
		removeSession(_sessions.front());
	}
	else
	{
		_sessions.front()->send(pkt::Ping);
		_pingTimer->expires_from_now(boost::posix_time::seconds(10));
		_pingTimer->async_wait(std::bind(&Room::onAliveCheck, this, std::placeholders::_1));
	}
}

Packet & operator<<(Packet & pkt, Room const & room)
{
	pkt << room._id
		<< ';' << room._name
		<< ';' << room._sessions.front()->name()
		<< ';' << room._sessions.front()->addr()
		<< ';' << std::boolalpha << !room._password.empty()
		<< ';' << room._gameMode
		<< ';' << room._map
		<< ';' << room._curPlayer
		<< ';' << room._maxPlayer;

	return pkt;
}
