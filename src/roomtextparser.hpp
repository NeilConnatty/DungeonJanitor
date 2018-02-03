#pragma once

#include "common.hpp"
#include "room.hpp"

class RoomParser
{
public:
	bool parseRoom(Room& room, const char* filename);

private:
	bool parseLine(std::string& line, float y);

private:
	std::vector<vec2> wall_pos;
	std::vector<vec2> floor_pos;
};