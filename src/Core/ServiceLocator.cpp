#include <OrbitalEncounters/Core/ServiceLocator.hpp>

std::unordered_map<std::type_index, std::unique_ptr<Service>> ServiceLocator::_services;