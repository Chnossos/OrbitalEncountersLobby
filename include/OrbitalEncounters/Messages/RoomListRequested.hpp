#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct RoomListRequested
	{
		Session::Ptr emitter;

		RoomListRequested(decltype(emitter) e)
			: emitter { e }
		{}
	};
}
