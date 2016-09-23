#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct RoomIsAlive
	{
		Session::Ptr host;

		RoomIsAlive(Session::Ptr s)
			: host { s }
		{}
	};
}
