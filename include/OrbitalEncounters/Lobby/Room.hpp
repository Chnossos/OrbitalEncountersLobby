#pragma once

#include <OrbitalEncounters/Network/Session.hpp>
#include <boost/asio/deadline_timer.hpp>
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
	std::uint8_t _curPlayer = 0;
	std::uint8_t _maxPlayer = 16;

	std::list<Session::Ptr> _sessions;

	std::unique_ptr<
		boost::asio::deadline_timer> _pingTimer;
	std::time_t                      _lastPongTime;

public:
	Room(Id const id, Session::Ptr owner);
	Room(Room &&) = default;
	~Room();

public:
	void addSession(Session::Ptr & s);
	void removeSession(Session::Ptr & s);
	void startAliveCheck();

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
	void setCurPlayer(std::uint8_t cur)     { _curPlayer = cur; }
	void setMaxPlayer(std::uint8_t max)     { _maxPlayer = max; }
	void updateLastPongTime()               { _lastPongTime = std::time(nullptr); }

private:
	void onAliveCheck(boost::system::error_code const & ec);

public:
	friend Packet & operator<<(Packet & p, Room const & r);
};