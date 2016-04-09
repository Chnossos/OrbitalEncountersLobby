#include <OrbitalEncounters/Network/SessionManager.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/SocketAccepted.hpp>

SessionManager::SessionManager()
{
	auto & tp = ServiceLocator::get<ThreadPool>();

	tp["App"].registerHandler<msg::SocketAccepted>(
		&SessionManager::onSocketAccepted, this);
}

void SessionManager::onSocketAccepted(Message<msg::SocketAccepted> msg)
{
	static Session::Id id = 0;

	auto session = std::make_shared<Session>(id++, std::move(msg->socket));

	_sessions.emplace(session->id(), session);

	session->run();
}