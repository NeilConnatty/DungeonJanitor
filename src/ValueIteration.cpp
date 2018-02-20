//Value.Iteration.cpp

#include "ValueIteration.hpp"

#define INITIAL_VALUE 0
#define NORMAL_ROOM_VALUE -0.04
#define BOSS_VALUE 1
#define ARTIFACT_VALUE 0.5
#define DISCOUNT_FACTOR 1
#define LOW_NUMBER_HACK -100

#include <fstream> // for testing
#include <string>// for testing

using namespace std;

//vector<Room>* ValueIteration::m_rooms;
map<Room*, float> ValueIteration::VI_current;
map<Room*, float> ValueIteration::VI_previous;

void ValueIteration::initialize(vector<Room> rooms)
{
	//m_rooms = &rooms;

	for (Room& room : rooms)
	{	
		Room* room_ptr = &room;
		
		float value = calculateInitialRoomValue(room_ptr);

		VI_current.emplace(room_ptr, value);

		//for testing
		printf("The initial value for this room is %f \n", value);
	}

	updateValues(rooms);
}

void ValueIteration::updateValues(vector<Room> rooms)
{
	VI_previous = VI_current;
	VI_current.clear();
	
	//for each room in VI_current, the new value for that room should be
	// the max value of each of that room's neighbors in VI_previous (V)
	// times the discount factor (g)
	// plus the room's inherent reward (R)
	// new_value = R + g * V;
	
	for (Room room : rooms)
	{
		float V = calculateHighestNeighborValue(&room);

		float new_value = calculateRoomReward(&room) + DISCOUNT_FACTOR * V;

		VI_current.emplace(&room, new_value);
		
		//for testing
		printf("The new value for this room is %f \n", new_value);
	}
}

Room * ValueIteration::getNextRoom(Room * current_room)
{
	float north_value = LOW_NUMBER_HACK;
	float south_value = LOW_NUMBER_HACK;
	float east_value = LOW_NUMBER_HACK;
	float west_value = LOW_NUMBER_HACK;

	// Load room values
	if (current_room->getNorthRoom() != nullptr)
	{
		north_value = VI_current.at(current_room->getNorthRoom());
	}
	if (current_room->getSouthRoom() != nullptr)
	{
		south_value = VI_current.at(current_room->getSouthRoom());
	}
	if (current_room->getEastRoom() != nullptr)
	{
		east_value = VI_current.at(current_room->getEastRoom());
	}
	if (current_room->getWestRoom() != nullptr)
	{
		west_value = VI_current.at(current_room->getWestRoom());
	}

	// Return best room
	if ((north_value != -100) && (north_value > south_value) && (north_value > east_value) && north_value > west_value)
	{
		return current_room->getNorthRoom();
	}
	
	if ((south_value != -100) && (south_value > north_value) && (south_value > east_value) && south_value > west_value)
	{
		return current_room->getSouthRoom();
	}
	
	if ((east_value != -100) && (east_value > south_value) && (east_value > north_value) && east_value > west_value)
	{
		return current_room->getEastRoom();
	}
	else
	{
		return current_room->getWestRoom();
	}
	
}

float ValueIteration::calculateHighestNeighborValue(Room * room)
{
	vector<float> neighbor_values;

	if (room->getNorthRoom() !=	nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->getNorthRoom()));
	}
	if (room->getSouthRoom() != nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->getSouthRoom()));
	}
	if (room->getEastRoom() != nullptr)
	{
		Room* eastRoom = room->getEastRoom();
		float eastValue = VI_previous.at(eastRoom);
		neighbor_values.push_back(eastValue);
	}
	if (room->getWestRoom() != nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->getWestRoom()));
	}

	return *max_element(neighbor_values.begin(), neighbor_values.end());;
}

float ValueIteration::calculateInitialRoomValue(Room * room)
{
	float reward = INITIAL_VALUE;

	if (room->containsBoss())
	{
		reward = BOSS_VALUE;
	}

	return reward;
}

float ValueIteration::calculateRoomReward(Room* room)
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

