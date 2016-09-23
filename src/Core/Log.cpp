#include <OrbitalEncounters/Core/Log.hpp>

std::mutex Log::_mutex;

/**
 * @param      os    Stream to output to. Defaults to @c std::cout.
 */
Log::Log(std::ostream & os)
: _os { os }
{}

/**
 * @details    Threadsafely output everything to the stream this
 *             object has been initialized with.
 */
Log::~Log()
{
	std::lock_guard<decltype(_mutex)> guard { _mutex };
	_os << str() << std::flush;
}
