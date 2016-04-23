#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct GameStart
	{
		Session::Ptr session;

		GameStart(decltype(session) s)
			: session { s }
		{}
	};
}