#pragma once

/// Base class for polymorphic use inside the ServiceLocator class.
struct Service
{
	/// Pure virtual destructor.
	virtual ~Service() = 0;
};
