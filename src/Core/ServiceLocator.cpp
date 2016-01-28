#include <OrbitalEncounters/Core/ServiceLocator.hpp>

std::shared_timed_mutex ServiceLocator::_mutex;
std::unordered_map<std::type_index, std::unique_ptr<Service>> ServiceLocator::_services;