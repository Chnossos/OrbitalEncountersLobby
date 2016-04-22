#pragma once

#define PACKET(name) constexpr auto const name = #name;

namespace pkt
{
	PACKET(CreateRoom);
	PACKET(ListRooms);
	PACKET(PlayerDisconnected);
	PACKET(WhatDoYouWant);
}

#undef PACKET