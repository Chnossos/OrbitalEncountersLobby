#pragma once

#define PACKET(name) constexpr auto const name = #name;

namespace pkt
{
	PACKET(AlreadyInARoom);
	PACKET(CreateRoom);
	PACKET(GameStart);
	PACKET(JoinRoom);
	PACKET(LeaveRoom);
	PACKET(ListRooms);
	PACKET(MyNameIs);
	PACKET(NotInARoom);
	PACKET(NotOwnerOfARoom);
	PACKET(PlayerJoined);
	PACKET(PlayerLeft);
	PACKET(RoomDisbanded);
	PACKET(RoomDoesNotExist);
	PACKET(RoomJoined);
	PACKET(Roommates);
	PACKET(WhatDoYouWant);
	PACKET(WhoAreYou);
}

#undef PACKET