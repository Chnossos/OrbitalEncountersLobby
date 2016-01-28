#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

class Log final : public std::ostringstream
{
private:
	static std::mutex _mutex;
	std::ostream & _os;

public:
	Log(std::ostream & os = std::cout);
	~Log();

public:
	Log(Log const &) = delete;
	Log(Log &&) = delete;
	Log & operator=(Log const &) = delete;
	Log & operator=(Log &&) = delete;
};
