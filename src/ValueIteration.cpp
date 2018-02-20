//Value.Iteration.cpp

#include "ValueIteration.hpp"

#define INITIAL_VALUE 0 // Fixed at 0
#define NORMAL_ROOM_VALUE -0.04 // Fixed at -0.04
#define BOSS_VALUE 1 // Fixed at 1
#define ARTIFACT_VALUE 0.50 // Fixed at 0.50
#define DISCOUNT_FACTOR 0.8 // Can play with
#define LOW_NUMBER_HACK -100

#include <fstream> // for testing
#include <string>// for testing

using namespace std;

//vector<Room>* ValueIteration::m_rooms;
//map<Room*, float> ValueIteration::VI_current;
//map<Room*, float> ValueIteration::VI_previous;
map<int, float> ValueIteration::VI_current;
map<int, float> ValueIteration::VI_previous;

void ValueIteration::initialize(vector<Room> rooms)
{
	//m_rooms = &rooms;

	for (Room& room : rooms)
	{	
		Room* room_ptr = &room;
		
		float value = calculateInitialRoomValue(room_ptr);

		VI_current.emplace(room_ptr->getRoomID(), value);

		//for testing
		printf("The initial value for room %d is %f \n", room_ptr->getRoomID(), value);
	}

	updateValues(rooms, 0.5);
	updateValues(rooms, 0.5);
	updateValues(rooms, 0.5);
	updateValues(rooms, 0.5);
	
}

void ValueIteration::updateValues(vector<Room> rooms, float artifact_probability)
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
		float new_value;

		if (room.containsBoss())
		{
			new_value = 1.0;
		}
		else
		{

			float V = calculateHighestNeighborValue(&room);

			new_value = calculateRoomReward(&room, artifact_probability) + DISCOUNT_FACTOR * V;
		}

		VI_current.emplace(room.getRoomID(), new_value);
		
		//for testing
		printf("The new value for room %d is %f \n", room.getRoomID(), new_value);
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
		north_value = VI_current.at(current_room->getNorthRoom()->getRoomID());
	}
	if (current_room->getSouthRoom() != nullptr)
	{
		south_value = VI_current.at(current_room->getSouthRoom()->getRoomID());
	}
	if (current_room->getEastRoom() != nullptr)
	{
		east_value = VI_current.at(current_room->getEastRoom()->getRoomID());
	}
	if (current_room->getWestRoom() != nullptr)
	{
		west_value = VI_current.at(current_room->getWestRoom()->getRoomID());
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
		neighbor_values.push_back(VI_previous.at(room->getNorthRoom()->getRoomID()));
	}
	if (room->getSouthRoom() != nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->getSouthRoom()->getRoomID()));
	}
	if (room->getEastRoom() != nullptr)
	{
		Room* eastRoom = room->getEastRoom();
		auto it = VI_previous.find(eastRoom->getRoomID());
		auto pair = *it;
		float eastValue = pair.second;
		neighbor_values.push_back(eastValue);
	}
	if (room->getWestRoom() != nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->getWestRoom()->getRoomID()));
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

float ValueIteration::calculateRoomReward(Room* room, float artifact_probability)
{
	float reward = NORMAL_ROOM_VALUE;

	if (room->containsBoss())
	{
		reward = BOSS_VALUE;
	}

	// If room has both boss and artifact, treated as only having Boss
	else if (room->containsArtifact())
	{
		reward = ARTIFACT_VALUE * artifact_probability;
	}

	return reward;

}

