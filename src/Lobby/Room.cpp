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

Room::~Room()
{
	Log {} << "R[" << _id << "] " << __FUNCTION__ << '\n';
}

void Room::addSession(Session::Ptr & session)
{
	Packet playerJoined { pkt::PlayerJoined };
	playerJoined << '|' << *session;

	// Notify PlayerJoined

	Packet roomJoined { pkt::RoomJoined };
	for (auto & s : _sessions)
	{
		roomJoined << '|' << *s;
		s->send(playerJoined);
	}

	_sessions.push_back(session);
	session->setRoom(this);

	// Notify RoomJoined + player list
	session->send(roomJoined);
}

void Room::removeSession(Session::Ptr & s)
{
	// Is this session really in this room ?
	auto it = std::find(std::cbegin(_sessions), std::cend(_sessions), s);
	if (it == std::cend(_sessions))
		return;

	bool wasHost = s == _sessions.front();

	s->setRoom(nullptr);
	_sessions.remove(s);

	if (wasHost) // Game no longer valid, disband
	{
		Packet packet { pkt::RoomDisbanded };
		
		for (auto & s : _sessions)
		{
			s->setRoom(nullptr);
			s->send(packet);
		}

		_sessions.clear();
	}

	if (_sessions.empty())
	{
		ServiceLocator::get<ThreadPool>()["App"].push<msg::EmptyRoom>(_id);
		return;
	}

	// Notify PlayerLeft to remaining players
	Packet packet { pkt::PlayerLeft };
	packet << '|' << *s;

	for (auto & s : _sessions)
		s->send(packet);
}

void Room::startGame() const
{
	for (auto & s : _sessions)
		s->send(pkt::GameStart);
}

Packet & operator<<(Packet & pkt, Room const & room)
{
	pkt << room._id
		<< ';' << room._name
		<< ';' << room._host
		<< ';' << room._password.empty()
		<< ';' << room._gameMode
		<< ';' << room._map
		<< ';' << room._maxPlayer;/* << ';';

	for (auto & session : room._sessions)
	{
		if (session != room._sessions.front())
			pkt << ',';

		pkt << *session;
	}*/

	return pkt;
}