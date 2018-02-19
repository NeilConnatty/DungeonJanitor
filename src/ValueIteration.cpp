//Value.Iteration.cpp

#include "ValueIteration.hpp"

#define INITIAL_VALUE -0.04
#define BOSS_VALUE 1
#define ARTIFACT_VALUE 0.7
#define DISCOUNT_FACTOR 1

void ValueIteration::initialize(vector<Room> rooms)
{
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
	//for each room in VI_current, the new value for that room should be
	// the max value of each of that room's neighbors in VI_previous (V)
	// times the discount factor (g)
	// plus the room's inherent Reward R
	// new_value = R + g * V;
	Room * room_ptr;
	
	//V = max()
	//float new_value = CalculateRoomReward(room_ptr) + DISCOUNT_FACTOR * V;
}

float ValueIteration::CalculateRoomReward(Room* room)
{
	float reward = INITIAL_VALUE;

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