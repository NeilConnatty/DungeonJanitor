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
	
	static void initialize(vector<Room> rooms);
	static void updateValues(vector<Room> rooms, float artifact_probability);
	static Room * getNextRoom(Room* current_room);


private:
	//static vector<Room>* m_rooms;
	//static map<Room*, float> VI_current;
	//static map<Room*, float> VI_previous;
	static map<int, float> VI_current;
	static map<int, float> VI_previous;

	static float calculateInitialRoomValue(Room* room);
	static float calculateRoomReward(Room* room, float artifact_probability);
	static float calculateHighestNeighborValue(Room* room);

};