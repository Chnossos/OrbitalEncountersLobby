#pragma once

#include <OrbitalEncounters/Network/Session.hpp>
#include <list>
#include <string>

struct Packet;

class Room final
{
public:
	using Id = std::uint32_t;

private:
	Id const     _id;
	std::string  _name;
	std::string  _password;
	std::uint8_t _gameMode = 0;
	std::uint8_t _map = 0;
	std::uint8_t _maxPlayer = 8;

	std::list<Session::Ptr> _sessions;

public:
	Room(Id const id, Session::Ptr owner);
	~Room();

public:
	void addSession(Session::Ptr & s);
	void removeSession(Session::Ptr & s);

public:
	auto id() const { return _id; }
	auto owner() const -> Session::Ptr const & {
		return _sessions.front();
	}

public:
	void setName(std::string const & n)     { _name = n; }
	void setPassword(std::string const & p) { _password = p; }
	void setGameMode(std::uint8_t gameMode) { _gameMode = gameMode; }
	void setMap(std::uint8_t map)           { _map = map; }
	void setMaxPlayer(std::uint8_t max)     { _maxPlayer = max; }

public:
	friend Packet & operator<<(Packet & p, Room const & r);
};