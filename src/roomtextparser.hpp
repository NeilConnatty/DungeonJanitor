#pragma once

#include "common.hpp"
#include "room.hpp"

class RoomParser
{
public:
	bool parseRoom(Room& room, const char* filename);

private:
	bool parseLine(std::string& line, float y, bool first_line);

private:
	std::vector<Room::wall_pair> wall_pairs;
	std::vector<vec2> floor_pos;
	std::vector<vec2> puddle_pos;
  std::vector<vec2> door_pos;
};
