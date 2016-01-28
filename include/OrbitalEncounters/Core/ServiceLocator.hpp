#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>

class ServiceLocator final
{
private:
	static std::shared_timed_mutex _mutex;
	static std::unordered_map<std::type_index, std::unique_ptr<Service>> _services;

public:
	template<typename T, typename... Args>
	static bool add(Args && ...args);

	template<typename T>
	static auto get() -> T &;
};

/* ****************************************************************************
** TEMPLATE IMPL
** ***************************************************************************/

template<typename T, typename... Args>
static bool ServiceLocator::add(Args && ...args)
{
	std::unique_lock<std::shared_timed_mutex> lock { _mutex };
	return _services.emplace
	(
		std::piecewise_construct,
		std::forward_as_tuple(typeid(T)),
		std::forward_as_tuple(std::make_unique<T>(std::forward<Args>(args)...))
	)
	.second;
}

template<typename T>
static auto ServiceLocator::get() -> T &
{
	std::shared_lock<std::shared_timed_mutex> lock { _mutex };
	return static_cast<T &>(*_services.at(typeid(T)));
}