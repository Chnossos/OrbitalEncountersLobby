#include <OrbitalEncounters/Core/ThreadGroup.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <functional>
#include <iostream>

/**
 * @brief      Constructor.
 *
 * @details    Start the given amount of threads.
 *
 * @param[in]  threadAmount  Number of thread to start.
 */
ThreadGroup::ThreadGroup(std::size_t threadAmount)
: MessageQueue { service }
, _work        { std::make_unique<boost::asio::io_service::work>(service) }
{
	_threads.reserve(threadAmount);
	while (threadAmount --> 0)
		_threads.emplace_back(std::bind(&ThreadGroup::run, this));
}

/**
 * @details    Unblock the @c io_service and join the threads.
 */
ThreadGroup::~ThreadGroup()
{
	// Should have been done before, but at this point
	// we're shutting down so the cost does not matter
	_work.reset();

	// Make sure we won't terminate of block
	finishAllWork();
}

/**
 * @details    Call @c io_service::run() and try to recover from exceptions.
 */
void ThreadGroup::run()
{
	do try
	{
		service.run();
	}
	catch (std::exception const & e)
	{
		Log { std::cerr } << "  [KO] Standard exception: " << e.what() << std::endl;
	}
	catch (...)
	{
		Log { std::cerr } << "  [KO] Unknown exception" << std::endl;
	}
	while (_work);

	Log {} << "  [OK] Thread id " << std::this_thread::get_id() << " has finished.\n";
}

/**
 * @details    Unblock the @c io_service to let it run out of work and exit
 *             gracefully.
 */
void ThreadGroup::shutdown()
{
	_work.reset();
}

void ThreadGroup::finishAllWork()
{
	for (auto & thread : _threads)
	{
		if (thread.joinable())
			thread.join();
	}
}
