#pragma once

#include <OrbitalEncounters/Network/Session.hpp>
#include <memory>
#include <string>

namespace msg
{
	struct CreateRoom
	{
		Session::Ptr session;
		std::string data;

		CreateRoom(decltype(session) owner, std::string data = "")
		: session { owner }
		, data    { data }
		{}
	};
}