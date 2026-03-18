#pragma once

#include <OrbitalEncounters/Core/MessageQueue.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>
#include <thread>
#include <vector>

/// A group of @c std::thread that shares a queue of tasks.
class ThreadGroup : public MessageQueue
{
public:
	// We won't ever have a side effect when accessing
	// so a direct getter is pointless, make it public
	boost::asio::io_context service;

private:
	/// Used to prevent the @c io_context from running out of work.
	bool _hasWork;

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
