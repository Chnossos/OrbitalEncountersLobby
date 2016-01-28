#include <OrbitalEncounters/Application.hpp>
#include <OrbitalEncounters/Core/Log.hpp>
#include <OrbitalEncounters/Core/ServiceLocator.hpp>
#include <OrbitalEncounters/Core/ThreadPool.hpp>
#include <OrbitalEncounters/Network/SocketListener.hpp>
#include <functional>
#include <iostream>

Application::Application()
{
	ServiceLocator::add<ThreadPool>();
}

void Application::run()
{
	auto & tp = ServiceLocator::get<ThreadPool>();
	tp.spawn({
		{ "App",     0 },
		{ "Network", 3 }
	});

	SocketListener listener { tp["Network"].service };
	if (!listener.listen(4242))
	{
		Log { std::cerr } << "Cannot start the server.\n";
		return;
	}

	Log {} << "System ready on *:4242\n";

	boost::asio::signal_set signalSet { tp["App"].service, SIGINT };
	signalSet.async_wait(std::bind(&Application::onSignal, this, std::placeholders::_1));

	Log {} << "Press CTRL-C to stop the application...\n";

	tp["App"].service.run();
}

void Application::onSignal(boost::system::error_code const &)
{
	ServiceLocator::get<ThreadPool>()["App"].shutdown();
}
