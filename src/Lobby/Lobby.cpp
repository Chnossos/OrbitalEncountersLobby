#include <OrbitalEncounters/Lobby/Lobby.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/CreateRoom.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Messages/JoinRoom.hpp>
#include <OrbitalEncounters/Messages/PlayerLeaving.hpp>
#include <OrbitalEncounters/Messages/RoomListRequested.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>

Lobby::Lobby()
{
	auto & tp = ServiceLocator::get<ThreadPool>();

	tp["App"].registerHandler<msg::CreateRoom>(&Lobby::onCreateRoom, this);
	tp["App"].registerHandler<msg::EmptyRoom>(&Lobby::onEmptyRoom, this);
	tp["App"].registerHandler<msg::JoinRoom>(&Lobby::onJoinRoom, this);
	tp["App"].registerHandler<msg::PlayerLeaving>(&Lobby::onLeavingRoom, this);
	tp["App"].registerHandler<msg::RoomListRequested>(&Lobby::onRoomListRequested, this);
	tp["App"].registerHandler<msg::SessionDisconnected>(&Lobby::onSessionDisconnected, this);
}

void Lobby::onCreateRoom(Message<msg::CreateRoom> msg)
{
	if (msg->session->room())
	{
		msg->session->send(pkt::AlreadyInARoom);
		return;
	}

	static Room::Id id = 0;

	Room r { id++, msg->session };

	/// TODO: Parse msg->data

	auto & room = _rooms.emplace(id - 1, std::move(r)).first->second;

	msg->session->setRoom(&room);
	msg->session->send(pkt::RoomJoined);

	Log {} << "Room " << room.id() << " created!\n";
}

void Lobby::onEmptyRoom(Message<msg::EmptyRoom> msg)
{
	auto it = _rooms.find(msg->roomId);
	if (it != _rooms.end())
	{
		Log {} << "Empty Room " << it->second.id() << " got deleted!\n";

		_rooms.erase(it);
	}
}

void Lobby::onJoinRoom(Message<msg::JoinRoom> msg)
{
	if (msg->session->room())
	{
		msg->session->send(pkt::AlreadyInARoom);
		return;
	}

	try
	{
		Room::Id id = std::stoi(msg->roomId.c_str());
		_rooms.at(id).addSession(msg->session);
	}
	catch (...) {
		msg->session->send(pkt::RoomDoesNotExist);
	}
}

void Lobby::onLeavingRoom(Message<msg::PlayerLeaving> msg)
{
	if (auto room = msg->player->room())
		room->removeSession(msg->player);
	else
		msg->player->send(pkt::NotInARoom);
}

void Lobby::onRoomListRequested(Message<msg::RoomListRequested> msg)
{
	/// TODO: cache room informations
	Packet packet { pkt::ListRooms };

	for (auto & it : _rooms)
		packet << '|' << it.second; /// TODO: complete data

	msg->emitter->send(packet);
}

void Lobby::onSessionDisconnected(Message<msg::SessionDisconnected> msg)
{
	if (auto room = msg->s->room())
		room->removeSession(msg->s);
}
