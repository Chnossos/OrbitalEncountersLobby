#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <OrbitalEncounters/Core/ThreadGroup.hpp>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <utility>

/// A pool of ThreadGroup, accessible by name.
class ThreadPool : public Service
{
	using Initializer = std::pair<std::string const, std::size_t>;

private:
	std::unordered_map<std::string, ThreadGroup> _threadGroups;

public:
	/// Spawn new ThreadGroups by specifiying their name and thread amount.
	void spawnThreadGroups(std::initializer_list<Initializer> list);

	/// Access a specific ThreadGroup by name.
	auto operator[](std::string const &) noexcept(false) -> ThreadGroup &;
};

/**
 * @param      name  Name of the ThreadGroup.
 *
 * @return     A reference to the group if found.
 */
inline auto ThreadPool::operator[](std::string const & name) -> ThreadGroup &
{
	return _threadGroups.at(name);
}
