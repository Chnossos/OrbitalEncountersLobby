#pragma once

#include <sstream>
#include <string>

/// Convenience wrapper around @c std::ostringstream.
struct Packet : public std::ostringstream
{
	/// Forward constructor.
	Packet(std::string const & header)
	: std::ostringstream(header, std::ios::ate)
	{}

	/// Forward constructor.
	Packet(char const * header)
	: std::ostringstream(header, std::ios::ate)
	{}

	/// Forward @c operator<<().
	template<typename T>
	Packet & operator<<(T && t)
	{
		static_cast<std::ostringstream &>(*this) << std::forward<T>(t);
		return *this;
	}

	/// Special treatment for @c std::uint8_t to avoid the conversion to a
	/// character litteral when using @c str().
	Packet & operator<<(std::uint8_t u)
	{
		static_cast<std::ostringstream &>(*this) << (std::uint16_t)u;
		return *this;
	}

	/// Special treatment for @c std::int8_t to avoid the conversion to a
	/// character litteral when using @c str().
	Packet & operator<<(std::int8_t u)
	{
		static_cast<std::ostringstream &>(*this) << (std::int16_t)u;
		return *this;
	}
};
