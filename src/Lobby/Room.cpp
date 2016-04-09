#include <OrbitalEncounters/Lobby/Room.hpp>

Room::Room(Id const id, Session::WPtr owner)
: _id       { id }
, _sessions { owner }
{}