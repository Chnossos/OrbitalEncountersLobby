#pragma once

#include <OrbitalEncounters/Network/Session.hpp>
#include <list>
#include <string>

class Room final
{
public:
	using Id = std::uint32_t;

private:
	Id const     _id;
	std::string  _name;
	std::string  _host;
	std::string  _password;
	std::uint8_t _gameMode;
	std::uint8_t _map;
	std::uint8_t _maxPlayer;

	std::list<Session::WPtr> _sessions;

public:
	Room(Id const id, Session::WPtr owner);

public:
	auto id() const { return _id; }
};