#pragma once

#include "common.hpp"
#include "room.hpp"
#include <map>
//#include <vector>
#include <algorithm>

using namespace std;


class ValueIteration
{
public:
	
	void initialize(vector<Room> rooms);
	void updateValues();


private:
	map<Room*, float> VI_current;
	map<Room*, float> VI_previous;

	float ValueIteration::CalculateRoomReward(Room* room);

};