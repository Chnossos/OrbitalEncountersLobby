#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>

class ServiceLocator final
{
	friend class Application;

private:
	static std::unordered_map<std::type_index, std::unique_ptr<Service>> _services;

private:
	template<typename T, typename... Args>
	static auto add(Args && ...args) -> T &;

	template<typename T>
	static void del();

public:
	template<typename T>
	static auto get() -> T &;
};

/* *****************************************************************************
** TEMPLATE IMPL
** ****************************************************************************/

template<typename T, typename... Args>
auto ServiceLocator::add(Args && ...args) -> T &
{
	return static_cast<T &>(*_services.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(typeid(T)),
		std::forward_as_tuple(std::make_unique<T>(std::forward<Args>(args)...))
	)
	.first->second);
}

template<typename T>
void ServiceLocator::del()
{
	_services.erase(typeid(T));
}

template<typename T>
auto ServiceLocator::get() -> T &
{
	return static_cast<T &>(*_services.at(typeid(T)));
}