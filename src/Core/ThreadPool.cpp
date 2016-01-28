#include <OrbitalEncounters/Core/ThreadPool.hpp>

ThreadPool::ThreadPool(std::initializer_list<Initializer> list)
{
	spawn(std::move(list));
}

void ThreadPool::spawn(std::initializer_list<Initializer> list)
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