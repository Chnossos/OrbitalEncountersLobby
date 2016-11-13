#include <iostream>

#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Lobby/Lobby.hpp>
#include <OrbitalEncounters/Network/SocketListener.hpp>
#include <OrbitalEncounters/Network/SessionManager.hpp>
#include <boost/asio/signal_set.hpp>
#include <clocale>
#include <csignal>
#include <functional>
#include <iostream>

/// Start and run the whole server machinery.
class Application final
{
	/// Nobody but @c main() can access this class.
	friend int main();

public:
	/// Constructor.
	Application();

	/// Destructor.
	~Application();

public:
	/// Run the server.
	void run();

private:
	/**
	 * @brief      Callback to handle system signals.
	 *
	 * @param      ec    Unused error code from boost.
	 */
	void onSignal(boost::system::error_code const & ec);
};

int main()
{
	// We want French accents working when the system locale is French
	std::setlocale(LC_ALL, "");

	Application {} .run();

#ifdef WIN32
	std::cout << "\nPress Enter key to exit...";
	std::cin.get();
#endif
}

/**
 * @details    Initialize global services.
 */
Application::Application()
{
	auto & tp = ServiceLocator::add<ThreadPool>();

	/// TODO: std::this_thread::hardware_concurrency balancing
	tp.spawnThreadGroups({
		{     "App", 0 },
		{ "Network", 3 }
	});

	// Don't forget to del() in reverse order in the destructor!

	ServiceLocator::add<SocketListener>(tp["Network"].service);
	ServiceLocator::add<SessionManager>();
	ServiceLocator::add<Lobby>();
}

/**
 * @details    Start listening to new clients and run the main thread worker.
 */
void Application::run()
{
	// If we can't do our server job, there is no point in continuing
	if (!ServiceLocator::get<SocketListener>().listen(4242))
	{
		Log { std::cerr } << "Cannot start the server.\n";
		return;
	}

	auto & tp = ServiceLocator::get<ThreadPool>();

	namespace pch = std::placeholders;
	boost::asio::signal_set signalSet { tp["App"].service, SIGINT };
	signalSet.async_wait(std::bind(&Application::onSignal, this, pch::_1));

	// Prep's done, now do some real work

	Log {} << "System ready on *:4242\n"
	       << "Press CTRL-C to stop the application...\n";
	tp["App"].service.run();

	// At this point we're shutting down

	Log {} << "Releasing the network threads.\n";
	tp["Network"].shutdown();
}

/**
 * @details    Shutdown and delete all global services.
 */
Application::~Application()
{
	// First remove all rooms
	Log {} << "Shutting down Lobby\n";
	ServiceLocator::del<Lobby>();

	// Now disconnect every client
	Log {} << "Shutting down SessionManager\n";
	ServiceLocator::del<SessionManager>();

	// All workers must finish before proceeding further.
	Log {} << "Shutting down ThreadPool\n";
	ServiceLocator::del<ThreadPool>();

	Log {} << "Shutting down done\n";
}

/**
 * @details    Shutdown the server when called whatever the error code value is.
 */
void Application::onSignal(boost::system::error_code const &)
{
	// Restore default signal action to be able to terminate if stuck
	std::signal(SIGINT, SIG_DFL);

	Log {} << "Shutdown requested...\n";

	Log {} << "Preventing new clients from connecting\n";
	ServiceLocator::get<SocketListener>().close();

	Log {} << "Releasing the main thread\n";
	ServiceLocator::get<ThreadPool>()["App"].shutdown();

	// Stop ping timers, shutdown socket, ...
	ServiceLocator::get<SessionManager>().shutdown();
}
