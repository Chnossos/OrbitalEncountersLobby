#pragma once

#define PACKET(name) constexpr auto const name = #name

namespace pkt
{
	PACKET(AlreadyInARoom);
	PACKET(CreateRoom);
	PACKET(JoinRoom);
	PACKET(LeaveRoom);
	PACKET(ListRooms);
	PACKET(MyNameIs);
	PACKET(NotInARoom);
	PACKET(NotOwnerOfARoom);
	PACKET(RoomCreationFailed);
	PACKET(RoomDisbanded);
	PACKET(RoomDoesNotExist);
	PACKET(RoomJoined);
	PACKET(WhoAreYou);
}

#undef PACKET
