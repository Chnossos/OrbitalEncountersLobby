#pragma once

#include <memory>
#include <string>

class Session;

namespace msg
{
	struct CreateRoom
	{
		std::weak_ptr<Session> session;
		std::string data;

		CreateRoom(decltype(session) owner, std::string data = "")
		: session { owner }
		, data    { data }
		{}
	};
}