#pragma once

#include <OrbitalEncounters/Network/Session.hpp>

namespace msg
{
	struct SessionDisconnected
	{
		Session::Ptr s;

		SessionDisconnected(decltype(s) s)
			: s { s }
		{}
	};
}