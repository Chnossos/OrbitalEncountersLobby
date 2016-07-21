#pragma once

#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct ConnectivityTestDone
	{
		Session::Ptr   host;
		Room::Id const roomId;
		bool const     success;

		ConnectivityTestDone(Session::Ptr s, Room::Id id, bool b)
			: host    { s  }
			, roomId  { id }
			, success { b  }
		{}
	};
}
