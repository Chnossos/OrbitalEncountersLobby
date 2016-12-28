#pragma once

#include <OrbitalEncounters/Core/MessageQueue.hpp>
#include <boost/asio/io_service.hpp>
#include <memory>
#include <thread>
#include <vector>

/// A group of @c std::thread that shares a queue of tasks.
class ThreadGroup : public MessageQueue
{
public:
	// We won't ever have a side effect when accessing
	// so a direct getter is pointless, make it public
	boost::asio::io_service service;

private:
	/// Used to prevent the @c io_service from running out of work.
	/// We can suppress this effect at any time by deleting the memory.
	std::unique_ptr<boost::asio::io_service::work> _work;

	/// We only add or remove everything at once
	/// so a vector is well-suited for the task.
	std::vector<std::thread> _threads;

public:
	/// Constructor.
	ThreadGroup(std::size_t threadAmount = 0);

	/// Destructor.
	~ThreadGroup();

public:
	/// Function executed by the threads.
	void run();

	/// Start the stopping of the workers.
	void shutdown();

	/// Wait for all threads to finish their work.
	void finishAllWork();
};
