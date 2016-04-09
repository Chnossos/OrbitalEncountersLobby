#include <OrbitalEncounters/Lobby/Lobby.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Messages/CreateRoom.hpp>

Lobby::Lobby()
{
	auto & tp = ServiceLocator::get<ThreadPool>();

	tp["App"].registerHandler<msg::CreateRoom>(&Lobby::onCreateRoom, this);
}

void Lobby::onCreateRoom(Message<msg::CreateRoom> msg)
{
	auto host = msg->session.lock();

	if (!host || _rooms.count(host->id()))
		return;

	_rooms.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(host->id()),
		std::forward_as_tuple(host->id(), msg->session)
	);
}