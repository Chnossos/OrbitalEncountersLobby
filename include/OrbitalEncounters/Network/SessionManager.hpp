#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Messages/Message.hpp>
#include <OrbitalEncounters/Network/Session.hpp>
#include <unordered_map>

namespace msg
{
	struct SessionDisconnected;
	struct SocketAccepted;
}

class SessionManager : public Service
{
private:
	std::unordered_map<Session::Id, Session::Ptr> _sessions;

public:
	SessionManager();
	~SessionManager();

private:
	void onSocketAccepted(Message<msg::SocketAccepted> msg);
	void onSessionDisconnected(Message<msg::SessionDisconnected> msg);
};