#pragma once

#include <OrbitalEncounters/Lobby/Room.hpp>

namespace msg
{
	struct EmptyRoom
	{
		Room::Id roomId;

		EmptyRoom(decltype(roomId) id)
			: roomId { id }
		{}
	};
}

