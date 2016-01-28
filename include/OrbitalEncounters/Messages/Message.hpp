#pragma once

#include <memory>

template<typename T>
using Message = std::shared_ptr<T>;
