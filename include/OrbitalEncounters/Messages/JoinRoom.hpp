#pragma once

#include <OrbitalEncounters/Network/Session.hpp>
#include <string>

namespace msg
{
	struct JoinRoom
	{
		Session::Ptr session;
		std::string  data;

		JoinRoom(decltype(session) s, decltype(data) id)
			: session { s }
			, data    { id }
		{}
	};
}