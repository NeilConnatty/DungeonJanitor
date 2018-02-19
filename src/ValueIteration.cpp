//Value.Iteration.cpp

#include "ValueIteration.hpp"

#define INITIAL_VALUE 0
#define NORMAL_ROOM_VALUE -0.04
#define BOSS_VALUE 1
#define ARTIFACT_VALUE 0.5
#define DISCOUNT_FACTOR 1

using namespace std;

void ValueIteration::initialize(vector<Room> rooms)
{
	m_rooms = rooms;

	for (Room& room : rooms)
	{	
		Room* room_ptr = &room;
		
		VI_current.emplace(room_ptr, CalculateRoomReward(room_ptr));
	}
}
//

void ValueIteration::updateValues()
{
	VI_previous = VI_current;
	VI_current.clear();
	
	//for each room in VI_current, the new value for that room should be
	// the max value of each of that room's neighbors in VI_previous (V)
	// times the discount factor (g)
	// plus the room's inherent Reward R
	// new_value = R + g * V;
	
	for (Room& room : m_rooms)
	{
		Room* room_ptr = &room;

		//list<float> neighbor_values;
		vector<float> neighbor_values;

		if (room_ptr->getNorthRoom() != nullptr)
		{
			neighbor_values.push_back(VI_previous.at(room_ptr->getNorthRoom()));
		}
		if (room_ptr->getSouthRoom() != nullptr)
		{
			neighbor_values.push_back(VI_previous.at(room_ptr->getSouthRoom()));
		}
		if (room_ptr->getEastRoom() != nullptr)
		{
			neighbor_values.push_back(VI_previous.at(room_ptr->getEastRoom()));
		}
		if (room_ptr->getWestRoom() != nullptr)
		{
			neighbor_values.push_back(VI_previous.at(room_ptr->getWestRoom()));
		}


		float V = *max_element(neighbor_values.begin(), neighbor_values.end());
		float new_value = CalculateRoomReward(room_ptr) + DISCOUNT_FACTOR * V;

		VI_current.emplace(room_ptr, new_value);
	}
}

float ValueIteration::CalculateInitialRoomValue(Room * room)
{
	float reward = INITIAL_VALUE;

	if (room->containsBoss())
	{
		reward = BOSS_VALUE;
	}

	return reward;
}

float ValueIteration::CalculateRoomReward(Room* room)
{
	float reward = NORMAL_ROOM_VALUE;

	if (room->containsBoss())
	{
		reward = BOSS_VALUE;
	}

	// If room has both boss and artifact, treated as only having Boss
	else if (room->containsArtifact())
	{
		reward = ARTIFACT_VALUE;
	}

	return reward;

}