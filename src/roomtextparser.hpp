#pragma once

#include "common.hpp"
#include "room.hpp"
using namespace std;

class RoomParser
{
public:
	bool parseRoom(Room& room, const char* filename);

private:
	void clearPositions();
	bool parseLine(string& line, float y, bool first_line);
	bool populateRoom(Room &room, vector<Room::wall_pair> wall_pairs, vector<vec2> floor_pos, vector<vec2> puddle_pos, vec2 hero_pos, vec2 boss_pos);

private:
	bool bossFound;
	bool heroFound;
	vector<Room::wall_pair> wall_pairs;
	vector<vec2> floor_pos;
	vector<vec2> puddle_pos;
	vec2 hero_pos;
	vec2 boss_pos;
};