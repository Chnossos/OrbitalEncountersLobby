#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packet.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>
#include <OrbitalEncounters/Network/Session.hpp>
#include <algorithm>
#include <iterator>

Room::Room(Id const id, SessionPtr owner)
: _id    { id }
, _owner { owner }
{}

Room::~Room()
{
	Log {} << "R[" << _id << "] " << __FUNCTION__ << '\n';
}

Packet & operator<<(Packet & pkt, Room const & room)
{
	pkt << room._id
		<< ';' << room._name
		<< ';' << room.owner()->name()
		<< ';' << room.owner()->addr()
		<< ';' << std::boolalpha << !room._password.empty()
		<< ';' << room._gameMode
		<< ';' << room._map
		<< ';' << room._curPlayer
		<< ';' << room._maxPlayer;

	return pkt;
}
