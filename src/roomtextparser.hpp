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

	bool has_artifact;
	bool has_boss_spawn;
	bool has_hero_spawn;
	bool has_janitor_spawn;

	vec2 artifact_pos;
	vec2 boss_spawn_pos;
	vec2 hero_spawn_pos;
	vec2 janitor_spawn_pos;
	
	vector<vec2> floor_pos;
	vector<vec2> puddle_pos;
	vector<vec2> door_pos;
	vector<Room::wall_pair> wall_pairs;
	

  
};

