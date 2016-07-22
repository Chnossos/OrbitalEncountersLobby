#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Messages/Message.hpp>
#include <unordered_map>

namespace msg
{
	struct ConnectivityTestDone;
	struct CreateRoom;
	struct EmptyRoom;
	struct JoinRoom;
	struct PlayerLeaving;
	struct RoomIsAlive;
	struct RoomListRequested;
	struct SessionDisconnected;
}

class Lobby final : public Service
{
private:
	std::unordered_map<Room::Id, Room> _rooms;
	std::unordered_map<Room::Id, Room> _pendingRooms;

public:
	Lobby();

private:
	void onConnectivityTestDone(Message<msg::ConnectivityTestDone> msg);
	void onCreateRoom(Message<msg::CreateRoom> msg);
	void onEmptyRoom(Message<msg::EmptyRoom> msg);
	void onJoinRoom(Message<msg::JoinRoom> msg);
	void onLeavingRoom(Message<msg::PlayerLeaving> msg);
	void onRoomIsAlive(Message<msg::RoomIsAlive> msg);
	void onRoomListRequested(Message<msg::RoomListRequested> msg);
	void onSessionDisconnected(Message<msg::SessionDisconnected> msg);

	void updateRoomInfo(Room & r, std::vector<std::string> const & data);
};