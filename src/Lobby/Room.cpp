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

#define PING_INTERVAL 10

namespace pch = std::placeholders;

Room::Room(Id const id, Session::Ptr owner)
: _id        { id }
, _sessions  { owner }
, _pingTimer {
	std::make_unique<boost::asio::deadline_timer>(
		ServiceLocator::get<ThreadPool>()["App"].service)
}
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
	auto it = std::find(_sessions.cbegin(), _sessions.cend(), s);
	if (it == _sessions.cend())
	{
		Log { std::cerr } << "ERROR Cannot remove session " << s->id()
		                  << " from room " << _id << ": session not found.\n";
	}
	else if (s == _sessions.front()) // Host is leaving, disband
	{
		for (auto & s : _sessions)
			s->setRoom(nullptr);

		_sessions.clear();
		ServiceLocator::get<ThreadPool>()["App"].push<msg::EmptyRoom>(_id);
	}
	else
	{
		_sessions.remove(s);
		s->setRoom(nullptr);
	}
}

void Room::startAliveCheck()
{
	_pingTimer->expires_from_now(boost::posix_time::seconds(PING_INTERVAL));
	_pingTimer->async_wait(std::bind(&Room::onAliveCheck, this, pch::_1));
	updateLastPongTime();
}

void Room::onAliveCheck(boost::system::error_code const & ec)
{
	if (ec == boost::asio::error::operation_aborted)
		return;

	if (ec)
	{
		Log { std::cerr } << "R[" << _id
		                  << "] something went wrong with the ping\n";
	}
	else if (std::time(nullptr) - _lastPongTime > PING_INTERVAL)
	{
		Log { std::cerr } << "R[" << _id
		                  << "] failed to respond to ping in time\n";

		// Host has become unresponsive, remove it completely
		removeSession(_sessions.front());
		ServiceLocator::get<ThreadPool>()["App"]
			.push<msg::SessionDisconnected>(_sessions.front());
	}
	else
	{
		// Everything's OK, ping again
		_sessions.front()->send(pkt::Ping);
		_pingTimer->expires_from_now(boost::posix_time::seconds(PING_INTERVAL));
		_pingTimer->async_wait(std::bind(&Room::onAliveCheck, this, pch::_1));
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
