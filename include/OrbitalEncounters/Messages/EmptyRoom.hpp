#pragma once

#include <OrbitalEncounters/Lobby/Room.hpp>

namespace msg
{
	struct EmptyRoom
	{
		Room::Ptr room;

		EmptyRoom(decltype(room) r)
			: room { r }
		{}
	};
}

