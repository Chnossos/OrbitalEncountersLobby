#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Network/Packet.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>
#include <algorithm>

Room::Room(Id const id, Session::Ptr owner)
: _id       { id }
, _sessions { owner }
{}

Room::~Room() = default;

void Room::addSession(Session::Ptr & session)
{
	_sessions.push_back(session);
	session->setRoom(this);
}

void Room::removeSession(Session::Ptr & s)
{
	// Is this session really in this room ?
	auto it = std::find(std::cbegin(_sessions), std::cend(_sessions), s);
	if (it == std::cend(_sessions)) return;

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

Packet & operator<<(Packet & pkt, Room const & room)
{
	pkt << room._id
		<< ';' << room._name
		<< ';' << room._sessions.front()->name()
		<< ';' << room._sessions.front()->addr()
		<< ';' << std::boolalpha << !room._password.empty()
		<< ';' << room._gameMode
		<< ';' << room._map
		<< ';' << room._sessions.size()
		<< ';' << room._maxPlayer;

	return pkt;
}