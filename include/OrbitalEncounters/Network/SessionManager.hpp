#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Messages/Message.hpp>
#include <OrbitalEncounters/Network/Session.hpp>
#include <unordered_map>

// Forward declarations to reduce include cluttering
namespace msg
{
	struct SessionDisconnected;
	struct SocketAccepted;
}

/// Container for sessions.
class SessionManager : public Service
{
private:
	std::unordered_map<Session::Id, Session::Ptr> _sessions;

public:
	/// Constructor.
	SessionManager();

	/// Destructor.
	~SessionManager();

public:
	void shutdown();

private:
	void onSocketAccepted(Message<msg::SocketAccepted> msg);
	void onSessionDisconnected(Message<msg::SessionDisconnected> msg);
};
