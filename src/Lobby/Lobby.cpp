#include <OrbitalEncounters/Lobby/Lobby.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/CreateRoom.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Messages/RoomListRequested.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>

Lobby::Lobby()
{
	auto & tp = ServiceLocator::get<ThreadPool>();

	tp["App"].registerHandler<msg::CreateRoom>(&Lobby::onCreateRoom, this);
	tp["App"].registerHandler<msg::EmptyRoom>(&Lobby::onEmptyRoom, this);
	tp["App"].registerHandler<msg::RoomListRequested>(&Lobby::onRoomListRequested, this);
	tp["App"].registerHandler<msg::SessionDisconnected>(&Lobby::onSessionDisconnected, this);
}

void Lobby::onCreateRoom(Message<msg::CreateRoom> msg)
{
	static Room::Id id = 0;

	Room r { id++, msg->session };

	/// TODO: Parse msg->data

	auto & room = _rooms.emplace(id - 1, std::move(r)).first->second;

	msg->session->setRoom(&room);

	Log {} << "Room " << room.id() << " created!\n";
}

void Lobby::onEmptyRoom(Message<msg::EmptyRoom> msg)
{
	auto it = _rooms.find(msg->roomId);
	if (it != _rooms.end())
	{
		Log {} << "Room " << it->second.id() << " is empty!\n";

		_rooms.erase(it);
		/// TODO: maybe send RoomRemoved to all listeners?
	}
}

void Lobby::onRoomListRequested(Message<msg::RoomListRequested> msg)
{
	Packet packet { pkt::ListRooms };

	for (auto & it : _rooms)
		packet << '|' << it.second.id(); /// TODO: complete data

	msg->emitter->send(packet);
}

void Lobby::onSessionDisconnected(Message<msg::SessionDisconnected> msg)
{
	if (auto room = msg->s->room())
		room->removeSession(msg->s);
}
