#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Core/ThreadGroup.hpp>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <utility>

/**
 ** Just a pool of ThreadGroup, accessible by name.
 */
class ThreadPool : public Service
{
	using Initializer = std::pair<std::string, std::size_t>;

private:
	std::unordered_map<std::string, ThreadGroup> _threadGroups;

public:
	ThreadPool() = default;
	ThreadPool(std::initializer_list<Initializer> list);

public:
	void spawn(std::initializer_list<Initializer> list);
	auto operator[](std::string const &) -> ThreadGroup &;
};

inline auto ThreadPool::operator[](std::string const & name) -> ThreadGroup &
{
	return _threadGroups.at(name);
}