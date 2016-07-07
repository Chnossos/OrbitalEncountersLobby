#pragma once

#include <algorithm>
#include <array>
#include <sstream>
#include <string>
#include <vector>

template<
	size_t   N,
	typename T = std::string,
	typename F = decltype(std::move<T const &>)>
auto split(std::string const & src, char sep, F && f = std::move)
{
	std::array<T, N> tok;

	if (!src.empty())
	{
		std::istringstream	iss { src };
		std::size_t			i = 0;
		std::string			str;

		while (i < N && std::getline(iss, str, sep))
			tok[i++] = f(str);
	}

	return tok;
}

template<
	typename T = std::string,
	typename F = decltype(std::move<T const &>)>
auto split (std::string const & src, char sep, F && f = std::move)
{
	std::vector<T> tok;

	if (!src.empty())
	{
		std::istringstream	iss { src };
		std::size_t			i = 0;
		std::string			str;

		tok.resize(std::count(std::begin(src), std::end(src), sep) + 1);
		while (std::getline(iss, str, sep))
			tok[i++] = f(str);
	}

	return tok;
}

auto split(std::string const & src, char sep)
{
	std::vector<std::string> tok;

	if (!src.empty())
	{
		std::istringstream	iss { src };
		std::size_t			i = 0;
		std::string			str;

		tok.resize(std::count(std::begin(src), std::end(src), sep) + 1);
		while (std::getline(iss, str, sep))
			tok[i++] = str;
	}

	return tok;
}