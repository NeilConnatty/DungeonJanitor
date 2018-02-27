#pragma once

#include "common.hpp"
#include "room.hpp"
#include <map>
#include <algorithm>
#include <list>

using namespace std;


class ValueIteration
{
public:
	
	static Room::directions getNextRoom(Room* current_room, vector<unique_ptr<Room>>& rooms, float artifact_probability);	

private:
	static float m_difference;
	static map<int, float> VI_current;
	static map<int, float> VI_previous;

	static float calculateInitialRoomValue(Room* room);
	static float calculateRoomReward(Room* room, float artifact_probability);
	static float calculateHighestNeighborValue(Room* room);
	static bool continueValueIterating();

	static void initialize(vector<unique_ptr<Room>>& rooms, float artifact_probability);
	static void updateValues(vector<unique_ptr<Room>>& rooms, float artifact_probability);
	static Room::directions getNextRoom(Room* current_room);

};
