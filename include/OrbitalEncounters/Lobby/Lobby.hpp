#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Messages/Message.hpp>
#include <memory>
#include <unordered_map>

// Forward-declaration to reduce include cluttering
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

/**
 * @brief      Basically a room manager.
 *
 * @details    It reacts to messages to create, manage and remove rooms from its
 *             internal collection.
 */
class Lobby final : public Service
{
private:
	std::unordered_map<Room::Id, Room::Ptr> _rooms;
	std::unordered_map<Room::Id, Room::Ptr> _pendingRooms;

public:
	/// Constructor.
	Lobby();

private: // Message handlers
	void onConnectivityTestDone(Message<msg::ConnectivityTestDone> msg);
	void onCreateRoom(Message<msg::CreateRoom> msg);
	void onEmptyRoom(Message<msg::EmptyRoom> msg);
	void onJoinRoom(Message<msg::JoinRoom> msg);
	void onLeavingRoom(Message<msg::PlayerLeaving> msg);
	void onRoomListRequested(Message<msg::RoomListRequested> msg);
	void onSessionDisconnected(Message<msg::SessionDisconnected> msg);
};
