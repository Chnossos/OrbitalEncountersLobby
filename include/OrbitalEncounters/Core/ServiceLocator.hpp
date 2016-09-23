#pragma once

#include <OrbitalEncounters/Core/Service.hpp>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>

/// Decoupling pattern: Provide a single access point for all services.
class ServiceLocator final
{
	/// The only class that can add or delete a service at runtime.
	friend class Application;

private:
	/// Mapping between a service type and its instance.
	static std::unordered_map<
		std::type_index, std::unique_ptr<Service>> _services;

private:
	/// Add a new service to the collection.
	template<typename T, typename... Args>
	static auto add(Args && ...args) -> T &;

	/// Delete the specified service from the collection.
	template<typename T>
	static void del();

public:
	/// Get a reference to the specified service from the collection.
	template<typename T>
	static auto get() -> T &;
};

/**
 * @param[in]  args  Arguments forwarded to the constructor of the service.
 *
 * @tparam     T          Service type.
 * @tparam     Args       Arguments type.
 *
 * @return     A reference to the old service if it exists else the new service.
 *
 * @remark     If a service of the same type already exists, it won't be
 *             replaced and must be deleted first.
 */
template<typename T, typename... Args>
auto ServiceLocator::add(Args && ... args) -> T &
{
	auto pair = _services.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(typeid(T)),
		std::forward_as_tuple(std::make_unique<T>(std::forward<Args>(args)...))
	);
	return static_cast<T &>(*pair.first->second);

}

/**
 * @tparam     T     Service type.
 */
template<typename T>
void ServiceLocator::del()
{
	_services.erase(typeid(T));
}

/**
 * @tparam     T     Service type.
 */
template<typename T>
auto ServiceLocator::get() -> T &
{
	return static_cast<T &>(*_services.at(typeid(T)));
}
