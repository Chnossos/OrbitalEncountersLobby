/**
 * @mainpage Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

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

#define TCP_LISTENING_PORT 4242
#define TO_STRING(ARG) #ARG

/// Start and run the whole server machinery.
class Application final
{
	/// Nobody but @c main() can access this class.
	friend int main();

	/// Constructor.
	Application();

	/// Destructor.
	~Application();

	/// Run the server.
	void run();
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

	Application().run();

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
	if (!ServiceLocator::get<SocketListener>().listen(TCP_LISTENING_PORT))
	{
		Log { std::cerr } << "Cannot start the server.\n";
		return;
	}

	auto & tp = ServiceLocator::get<ThreadPool>();

	namespace pch = std::placeholders;
	boost::asio::signal_set signalSet { tp["App"].service, SIGINT };
	signalSet.async_wait(std::bind(&Application::onSignal, this, pch::_1));

	// Prep's done, now do some real work

	Log {} << "System ready on *:" << TCP_LISTENING_PORT << '\n'
	       << "Press CTRL-C to stop the application...\n";
	tp["App"].service.run();

	// At this point we're shutting down

	Log {} << "[..] Releasing the network threads...\n";
	tp["Network"].shutdown();
	tp["Network"].finishAllWork();
}

/**
 * @details    Shutdown and delete all global services.
 */
Application::~Application()
{
	// First remove all rooms
	Log {} << "[  ] Shutting down Lobby";
	ServiceLocator::del<Lobby>();
	Log {} << "\r[OK] Shutting down Lobby\n";

	// Now disconnect every client
	Log {} << "[  ] Shutting down SessionManager";
	ServiceLocator::del<SessionManager>();
	Log {} << "\r[OK] Shutting down SessionManager\n";

	// All workers must finish before proceeding further.
	Log {} << "[  ] Shutting down ThreadPool";
	ServiceLocator::del<ThreadPool>();
	Log {} << "\r[OK] Shutting down ThreadPool\n";

	Log {} << "\n[APPLICATION SHUTDOWN REQUESTED - FINISHED]\n";
}

/**
 * @details    Shutdown the server when called whatever the error code value is.
 */
void Application::onSignal(boost::system::error_code const &)
{
	// Restore default signal action to be able to terminate if stuck
	std::signal(SIGINT, SIG_DFL);

	Log {} << "\n[APPLICATION SHUTDOWN REQUESTED - STARTING OPERATIONS]\n\n";

	Log {} << "[  ] Preventing new clients from connecting";
	ServiceLocator::get<SocketListener>().close();
	Log {} << "\r[OK] Preventing new clients from connecting\n";

	Log {} << "[  ] Releasing the main thread";
	ServiceLocator::get<ThreadPool>()["App"].shutdown();
	Log {} << "\r[OK] Releasing the main thread\n";

	// Stop ping timers, shutdown sockets, ...
	ServiceLocator::get<SessionManager>().shutdown();
}
