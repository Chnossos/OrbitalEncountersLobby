#pragma once

#include <OrbitalEncounters/Lobby/Room.hpp>
#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct ConnectivityTestDone
	{
		Session::Ptr host;
		bool const   success;

		ConnectivityTestDone(Session::Ptr s, bool b)
			: host    { s }
			, success { b }
		{}
	};
}
