#pragma once

#include <list>
#include <memory>
#include <string>

struct Packet;
class Session;

/// Mirror the lobby in the Unity Client to react accordingly.
class Room final : public std::enable_shared_from_this<Room>
{
public:
	using Id  = std::uint32_t;
	using Ptr = std::shared_ptr<Room>;

	using SessionPtr = std::shared_ptr<Session>;

private:
	Id const     _id;
	std::string  _name;
	std::string  _password;
	std::uint8_t _gameMode = 0;
	std::uint8_t _map = 0;
	std::uint8_t _curPlayer = 0;
	std::uint8_t _maxPlayer = 16;

	// First session in the list is the host
	std::list<SessionPtr> _sessions;

public:
	/// Constructor.
	Room(Id const id, SessionPtr owner);

	/// Force-enable move
	Room(Room &&) = default;

	/// Destructor
	~Room();

public:
	/// Add a client to the room.
	void addSession(SessionPtr & s);

	/// Remove a client from the room.
	void removeSession(SessionPtr & s);

public:
	/// Get the unique identifier of this room.
	auto id() const {
		return _id;
	}

	/// Get the pointer to the host of the room.
	auto owner() const -> SessionPtr const & {
		return _sessions.front();
	}

public:
	/// Set the name of the room.
	void setName(std::string const & n) {
		_name = n;
	}

	/// Set the password of the room.
	void setPassword(std::string const & p) {
		_password = p;
	}

	/// Set the game mode for this room.
	void setGameMode(std::uint8_t gameMode) {
		_gameMode = gameMode;
	}

	/// Set the map for this room.
	void setMap(std::uint8_t map) {
		_map = map;
	}

	/// Update the amount of players in this room.
	void setCurPlayer(std::uint8_t cur) {
		_curPlayer = cur;
	}

	/// Set the maximum amount of players this room can accept.
	void setMaxPlayer(std::uint8_t max) {
		_maxPlayer = max;
	}

public:
	/// Format the room data in a network packet.
	friend Packet & operator<<(Packet & p, Room const & r);
	friend Packet & operator<<(Packet & p, Room & r);
};

/// We need this version to not let the compiler choose @c Packet::operator<<.
inline Packet & operator<<(Packet & pkt, Room & room) {
	return pkt << static_cast<Room const &>(room);
}
