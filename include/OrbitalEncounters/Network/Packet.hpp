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

	template<typename T>
	Packet & operator<<(T const & t)
	{
		static_cast<std::ostringstream &>(*this) << t;
		return *this;
	}

	Packet & operator<<(std::uint8_t u)
	{
		static_cast<std::ostringstream &>(*this) << (std::uint16_t)u;
		return *this;
	}
};
