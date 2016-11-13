#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packet.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>
#include <OrbitalEncounters/Network/Session.hpp>
#include <algorithm>
#include <iterator>

Room::Room(Id const id, SessionPtr owner)
: _id        { id }
, _sessions  { owner }
{}

Room::~Room()
{
	Log {} << "R[" << _id << "] " << __FUNCTION__ << '\n';
}

void Room::addSession(SessionPtr & session)
{
	_sessions.push_back(session);
	session->setRoom(shared_from_this());
}

void Room::removeSession(SessionPtr & s)
{
	auto it = std::find(_sessions.cbegin(), _sessions.cend(), s);
	if (it == _sessions.cend())
	{
		Log { std::cerr } << "ERROR: Cannot remove session " << s->id()
		                  << " from room " << _id << ": session not found.\n";
	}
	else if (s == _sessions.front()) // Host is leaving, disband
	{
		Log {} << "Host is leaving, disbanding room " << _id << '\n';
		for (auto & s : _sessions)
			s->setRoom(nullptr);

		_sessions.clear();
		ServiceLocator::get<ThreadPool>()["App"].push<msg::EmptyRoom>(shared_from_this());
	}
	else
	{
		_sessions.remove(s);
		s->setRoom(nullptr);
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
