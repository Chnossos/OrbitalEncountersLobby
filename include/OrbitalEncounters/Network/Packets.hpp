#pragma once

#define PACKET(name) constexpr auto const name = #name;

namespace pkt
{
	PACKET(AlreadyInARoom);
	PACKET(CreateRoom);
	PACKET(JoinRoom);
	PACKET(LeaveRoom);
	PACKET(ListRooms);
	PACKET(NotInARoom);
	PACKET(PlayerJoined);
	PACKET(PlayerLeft);
	PACKET(RoomDisbanded);
	PACKET(RoomDoesNotExist);
	PACKET(RoomJoined);
	PACKET(Roommates);
	PACKET(WhatDoYouWant);
}

#undef PACKET