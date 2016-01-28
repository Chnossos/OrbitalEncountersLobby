#include <OrbitalEncounters/Core/Log.hpp>

std::mutex Log::_mutex;

Log::Log(std::ostream & os)
: _os { os }
{}

Log::~Log()
{
	std::lock_guard<std::mutex> guard { _mutex };
	_os << str() << std::flush;
}
