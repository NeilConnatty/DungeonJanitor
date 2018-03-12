#pragma once

#include "common.hpp"
#include "room.hpp"
#include "dungeon.hpp"

#include <map>
#include <algorithm>
#include <list>
#include <cmath>

using namespace std;


class ValueIteration
{
public:
	
	static Room::adjacent_room getNextRoom(const Room* current_room, vector<unique_ptr<Room>>& rooms, float artifact_probability, Dungeon& dungeon);	

private:
	static float m_difference;
	static map<int, float> VI_current;
	static map<int, float> VI_previous;

	static float calculateInitialRoomValue(Room* room);
	static float calculateRoomReward(Room* room, float artifact_probability);
	static float calculateHighestNeighborValue(Room* room, Dungeon& dungeon);
	static bool continueValueIterating();

	static void initialize(vector<unique_ptr<Room>>& rooms, float artifact_probability, Dungeon& dungeon);
	static void updateValues(vector<unique_ptr<Room>>& rooms, float artifact_probability, Dungeon& dungeon);
	static Room::adjacent_room getNextRoom(const Room* current_room, Dungeon& dungeon);

};
