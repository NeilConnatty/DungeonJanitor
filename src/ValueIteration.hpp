#pragma once

#include "common.hpp"
#include "room.hpp"
#include <map>
//#include <vector>
#include <algorithm>
#include <list>

using namespace std;


class ValueIteration
{
public:
	
	void initialize(vector<Room> rooms);
	void updateValues();
	Room * getNextRoom(Room* current_room);


private:
	vector<Room> m_rooms;
	map<Room*, float> VI_current;
	map<Room*, float> VI_previous;

	float calculateInitialRoomValue(Room* room);
	float calculateRoomReward(Room* room);
	float calculateHighestNeighborValue(Room* room);

};