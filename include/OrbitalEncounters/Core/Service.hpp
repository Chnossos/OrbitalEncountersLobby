#pragma once

struct Service
{
	Service() noexcept = default;
	Service(Service const &) = default;
	Service(Service &&) = default;
	Service & operator=(Service const &) = default;
	Service & operator=(Service &&) = default;
	virtual ~Service() = 0;
};