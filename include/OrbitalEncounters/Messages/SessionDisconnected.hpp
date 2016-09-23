#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct SessionDisconnected
	{
		Session::Ptr session;

		SessionDisconnected(decltype(session) s)
			: session { s }
		{}
	};
}
