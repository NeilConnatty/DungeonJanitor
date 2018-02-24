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
	bool populateRoom(Room &room);

private:
	vector<Room::wall_pair> wall_pairs;
	vector<vec2> floor_pos;
	vector<vec2> puddle_pos;
	bool has_artifact;
	vec2 artifact_pos;
	bool has_boss_spawn;
	vec2 boss_spawn_pos;
	bool has_hero_spawn;
	vec2 hero_spawn_pos;
	bool has_janitor_spawn;
	vec2 janitor_spawn_pos;
};