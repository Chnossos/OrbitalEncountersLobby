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
	std::string  _host;
	std::string  _password;
	std::uint8_t _gameMode = 0;
	std::uint8_t _map = 0;
	std::uint8_t _maxPlayer = 2;

	std::list<Session::Ptr> _sessions;

public:
	Room(Id const id, Session::Ptr owner);
	~Room();

public:
	void addSession(Session::Ptr & s);
	void removeSession(Session::Ptr & s);
	void startGame() const;
	auto owner() -> Session::Ptr & {
		return _sessions.front();
	}

public:
	auto id() const { return _id; }

public:
	friend Packet & operator<<(Packet & p, Room const & r);
};