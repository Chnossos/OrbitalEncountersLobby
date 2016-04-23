#pragma once

#include <OrbitalEncounters/Network/Session.hpp>
#include <string>

namespace msg
{
	struct JoinRoom
	{
		Session::Ptr session;
		std::string  roomId;

		JoinRoom(decltype(session) s, decltype(roomId) id)
			: session { s }
			, roomId  { id }
		{}
	};
}