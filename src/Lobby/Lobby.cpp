#include <OrbitalEncounters/Lobby/Lobby.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/ConnectivityTestDone.hpp>
#include <OrbitalEncounters/Messages/CreateRoom.hpp>
#include <OrbitalEncounters/Messages/EmptyRoom.hpp>
#include <OrbitalEncounters/Messages/JoinRoom.hpp>
#include <OrbitalEncounters/Messages/PlayerLeaving.hpp>
#include <OrbitalEncounters/Messages/RoomListRequested.hpp>
#include <OrbitalEncounters/Messages/SessionDisconnected.hpp>
#include <OrbitalEncounters/Network/Packets.hpp>
#include <OrbitalEncounters/Utility/Split.hpp>
#include <boost/lexical_cast.hpp>

Lobby::Lobby()
{
	auto & tp = ServiceLocator::get<ThreadPool>();

	tp["App"].registerHandler<msg::ConnectivityTestDone>(&Lobby::onConnectivityTestDone, this);
	tp["App"].registerHandler<msg::CreateRoom>(&Lobby::onCreateRoom, this);
	tp["App"].registerHandler<msg::EmptyRoom>(&Lobby::onEmptyRoom, this);
	tp["App"].registerHandler<msg::JoinRoom>(&Lobby::onJoinRoom, this);
	tp["App"].registerHandler<msg::PlayerLeaving>(&Lobby::onLeavingRoom, this);
	tp["App"].registerHandler<msg::RoomListRequested>(&Lobby::onRoomListRequested, this);
	tp["App"].registerHandler<msg::SessionDisconnected>(&Lobby::onSessionDisconnected, this);
}

void Lobby::onConnectivityTestDone(Message<msg::ConnectivityTestDone> msg)
{
	if (msg->success)
	{
		auto p = _rooms.emplace(msg->roomId, std::move(_pendingRooms.at(msg->roomId)));
		_pendingRooms.erase(msg->roomId);
		msg->host->setRoom(&p.first->second); // The data moved so we need to update!
		msg->host->send(pkt::RoomJoined);
	}
	else
	{
		_pendingRooms.erase(msg->roomId);
		msg->host->setRoom(nullptr);
	}
}

void Lobby::onCreateRoom(Message<msg::CreateRoom> msg) try
{
	static Room::Id id = 0;
	std::vector<std::string> data = split(msg->data, ';');

	if (msg->session->room() != nullptr)
	{
		updateRoomInfo(*msg->session->room(), data);
		Log {} << "Room " << msg->session->room()->id() << " updated!\n";
	}
	else
	{
		Room r { id++, msg->session };
		updateRoomInfo(r, data);

		auto & room = _pendingRooms.emplace(id - 1, std::move(r)).first->second;
		msg->session->setRoom(&room);
		msg->session->testUDPConnectivity();

		Log {} << "Room " << room.id() << " created!\n";
	}
}
catch (...)
{
	Log { std::cerr } << "Could not parse the room parameters\n";
	msg->session->send(pkt::RoomCreationFailed);
}

void Lobby::onEmptyRoom(Message<msg::EmptyRoom> msg)
{
	auto it = _rooms.find(msg->roomId);
	if (it != _rooms.end())
		_rooms.erase(it);
	else
		Log {} << "Room " << msg->roomId << " should be empty but could not be found";
}

void Lobby::onJoinRoom(Message<msg::JoinRoom> msg) try
{
	auto const table = split(msg->data, ';');
	auto const id = boost::lexical_cast<Room::Id>(table[0]);

	if (msg->session->room() != nullptr) {
		msg->session->send(pkt::AlreadyInARoom);
	}
	else if (boost::lexical_cast<bool>(table[1])) {
		_rooms.at(id).addSession(msg->session);
		msg->session->send(pkt::RoomJoined);
	}
	else {
		_rooms.at(id).owner()->send(pkt::RoomUnreachable);
	}
}
catch (std::out_of_range const &) {
	msg->session->send(pkt::RoomDoesNotExist);
}
catch (...) {
	Log { std::cerr } << "Session " << msg->session->id() << " failed to join a room\n";
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
	Packet roomList { pkt::ListRooms };

	for (auto & it : _rooms)
		roomList << '|' << it.second;

	msg->emitter->send(roomList);
}

void Lobby::onSessionDisconnected(Message<msg::SessionDisconnected> msg)
{
	if (auto room = msg->s->room())
		room->removeSession(msg->s);
}

void Lobby::updateRoomInfo(Room & r, std::vector<std::string> const & data)
{
	r.setName(data.at(0));
	r.setPassword(data.at(1));
	r.setGameMode(static_cast<std::uint8_t>(std::stoi(data.at(2))));
	r.setMap(static_cast<std::uint8_t>(std::stoi(data.at(3))));
	r.setCurPlayer(static_cast<std::uint8_t>(std::stoi(data.at(4))));
	r.setMaxPlayer(static_cast<std::uint8_t>(std::stoi(data.at(5))));
}
