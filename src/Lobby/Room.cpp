#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Network/Packet.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>

Room::Room(Id const id, Session::Ptr owner)
: _id       { id }
, _sessions({ owner })
{}

void Room::removeSession(Session::Ptr s)
{
	_sessions.remove(s);

	if (_sessions.empty())
	{
		ServiceLocator::get<ThreadPool>()["App"].push<msg::EmptyRoom>(_id);
		return;
	}

	Packet packet { pkt::PlayerDisconnected };
	packet << '|' << s->id();

	for (auto & s : _sessions)
		s->send(packet);
}
