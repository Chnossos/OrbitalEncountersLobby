#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct SessionIsAlive
	{
		Session::Ptr host;

		SessionIsAlive(Session::Ptr s)
			: host { s }
		{}
	};
}
