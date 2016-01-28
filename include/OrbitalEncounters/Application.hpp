#pragma once

#include <OrbitalEncounters/Core/Singleton.hpp>
#include <boost/asio/signal_set.hpp>

class Application : public Singleton<Application>
{
	friend class Singleton<Application>;

private:
	Application();

public:
	void run();

private:
	void onSignal(boost::system::error_code const & ec);
};