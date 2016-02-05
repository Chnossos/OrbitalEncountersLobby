#include <iostream>

#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Network/SocketListener.hpp>
#include <OrbitalEncounters/SessionManager.hpp>
#include <boost/asio/signal_set.hpp>
#include <clocale>
#include <csignal>
#include <functional>
#include <iostream>

// Nobody but main is supposed to access this class
class Application final
{
public:
	Application();
	~Application();

public:
	void run();

private:
	void onSignal(boost::system::error_code const & ec);
};

int main()
{
	// We want French accents working...
	std::setlocale(LC_ALL, "");

	Application {} .run();

	std::cout << "Press Enter key to exit...";
	std::cin.get();
}

Application::Application()
{
	ServiceLocator::add<ThreadPool>();

	auto & tp = ServiceLocator::get<ThreadPool>();
	tp.spawn({
		{     "App", 0 },
		{ "Network", 3 }
	});

	ServiceLocator::add<SocketListener>(tp["Network"].service);
	ServiceLocator::add<SessionManager>();
}

void Application::run()
{
	if (!ServiceLocator::get<SocketListener>().listen(4242))
	{
		Log { std::cerr } << "Cannot start the server.\n";
		return;
	}

	Log {} << "System ready on *:4242\n";

	auto & tp = ServiceLocator::get<ThreadPool>();

	boost::asio::signal_set signalSet { tp["App"].service, SIGINT };
	signalSet.async_wait(std::bind(&Application::onSignal, this, std::placeholders::_1));

	Log {} << "Press CTRL-C to stop the application...\n";

	tp["App"].service.run();

	Log {} << "Releasing the network threads.\n";
	tp["Network"].shutdown();
}

Application::~Application()
{

	ServiceLocator::del<SessionManager>();
	ServiceLocator::del<ThreadPool>();
}

void Application::onSignal(boost::system::error_code const &)
{
	Log {} << "Shutdown requested...\n";

	Log {} << "Closing the SocketListener\n";
	ServiceLocator::get<SocketListener>().close();

	Log {} << "Releasing the main thread\n";
	ServiceLocator::get<ThreadPool>()["App"].shutdown();
}
