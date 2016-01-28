#include <OrbitalEncounters/Core/ThreadGroup.hpp>
#include <functional>
#include <iostream>

ThreadGroup::ThreadGroup(std::size_t amount)
: MessageQueue { service }
, _work { std::make_unique<boost::asio::io_service::work>(service) }
{
	_threads.reserve(amount);
	while (amount --> 0)
		_threads.emplace_back(std::bind(&ThreadGroup::run, this));
}

/**
 ** Make sure we won't terminate or block.
 */
ThreadGroup::~ThreadGroup()
{
	// Should have been done before, but at this point
	// we're shutting down so the cost does not matter
	_work.reset();

	for (auto & thread : _threads)
	{
		if (thread.joinable())
			thread.join();
	}
}

void ThreadGroup::run()
{
	do try
	{
		service.run();
	}
	catch (std::exception const & e)
	{
		std::cerr << "Standard exception: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
	}
	while (_work);
}

void ThreadGroup::shutdown()
{
	_work.reset();
}