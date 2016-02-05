#pragma once

#include <sstream>
#include <string>

struct Packet : public std::ostringstream
{
	Packet(std::string const & header)
	: std::ostringstream(header, std::ios::ate)
	{}

	Packet(char const * header)
	: std::ostringstream(header, std::ios::ate)
	{}
};
