#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

/**
 * @brief      Threadsafe output stream.
 *
 * @par Example
 *     @code{.cpp}
 *     Log {} << "This string goes to stdout\n";
 *     Log { std::cerr } << "This string goes to stderr\n";
 *     @endcode
 */
class Log final : public std::ostringstream
{
private:
	/// Shared by all Log objects.
	static std::mutex _mutex;

	/// Reference to the stream to output to, no ownership taken.
	std::ostream & _os;

public:
	/// Constructor.
	Log(std::ostream & os = std::cout);

	/// Destructor.
	~Log();

private: // Private means no documentation needed for each deleted member
	// This class is not meant to be copied/moved.
	Log(Log const &) = delete;
	Log(Log &&) = delete;
	Log & operator=(Log const &) = delete;
	Log & operator=(Log &&) = delete;
};
