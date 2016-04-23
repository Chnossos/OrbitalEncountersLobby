#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Messages/Message.hpp>
#include <unordered_map>

namespace msg
{
	struct CreateRoom;
	struct EmptyRoom;
	struct GameStart;
	struct JoinRoom;
	struct PlayerLeaving;
	struct RoomListRequested;
	struct SessionDisconnected;
}

class Lobby final : public Service
{
private:
	std::unordered_map<Room::Id, Room> _rooms;

public:
	Lobby();

private:
	void onCreateRoom(Message<msg::CreateRoom> msg);
	void onEmptyRoom(Message<msg::EmptyRoom> msg);
	void onGameStart(Message<msg::GameStart> msg);
	void onJoinRoom(Message<msg::JoinRoom> msg);
	void onLeavingRoom(Message<msg::PlayerLeaving> msg);
	void onRoomListRequested(Message<msg::RoomListRequested> msg);
	void onSessionDisconnected(Message<msg::SessionDisconnected> msg);
};