#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct PlayerLeaving
	{
		Session::Ptr player;

		PlayerLeaving(decltype(player) p)
			: player { p }
		{}
	};
}