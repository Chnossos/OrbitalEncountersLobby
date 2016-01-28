#pragma once

#include <OrbitalEncounters/Core/MessageQueue.hpp>
#include <boost/asio/io_service.hpp>
#include <memory>
#include <thread>
#include <vector>

class ThreadGroup : public MessageQueue
{
public:
	// We won't ever have a side effect when accessing
	// so a direct getter is pointless, make it public
	boost::asio::io_service service;

private:
	std::unique_ptr<boost::asio::io_service::work> _work;
	std::vector<std::thread> _threads;

public:
	ThreadGroup(std::size_t amount = 0);
	~ThreadGroup();

public:
	void run();
	void shutdown();
};
