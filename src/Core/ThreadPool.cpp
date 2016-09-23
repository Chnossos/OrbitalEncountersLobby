#include <OrbitalEncounters/Core/ThreadPool.hpp>

/**
 * @param[in]  list  List of ThreadGroup initializers.
 *
 * @remark     An existing group cannot be replaced or deleted.
 */
void ThreadPool::spawnThreadGroups(std::initializer_list<Initializer> list)
{
	for (auto const & pair : list)
	{
		_threadGroups.emplace
		(
			std::piecewise_construct,
			std::forward_as_tuple(std::move(pair.first)),
			std::forward_as_tuple(pair.second)
		);
	}
}
