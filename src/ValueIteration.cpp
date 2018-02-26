//Value.Iteration.cpp

#include "ValueIteration.hpp"

#define INITIAL_VALUE 0 // Fixed at 0
#define NORMAL_ROOM_VALUE -0.04 // Fixed at -0.04
#define BOSS_VALUE 1 // Fixed at 1
#define ARTIFACT_VALUE 0.50 // Fixed at 0.50
#define DISCOUNT_FACTOR 0.8 // Can play with
#define LOW_NUMBER_HACK -100
#define MAX_ERROR 0.01 // Can play with

#include <fstream> // for testing
#include <string>// for testing

using namespace std;

//vector<Room>* ValueIteration::m_rooms;
//map<Room*, float> ValueIteration::VI_current;
//map<Room*, float> ValueIteration::VI_previous;
float ValueIteration::m_difference;
map<int, float> ValueIteration::VI_current;
map<int, float> ValueIteration::VI_previous;


Room::directions ValueIteration::getNextRoom(Room * current_room, vector<unique_ptr<Room>>& rooms, float artifact_probability)
{
	initialize(rooms, artifact_probability);
	return getNextRoom(current_room);
}

void ValueIteration::initialize(vector<unique_ptr<Room>>& rooms, float artifact_probability)
{
	//m_rooms = &rooms;

	for (unique_ptr<Room>& room : rooms)
	{	
		Room* room_ptr = room.get();
		
		float value = calculateInitialRoomValue(room_ptr);

		VI_current.emplace(room_ptr->getRoomID(), value);

		//for testing
		printf("The initial value for room %d is %f \n", room_ptr->getRoomID(), value);
	}

	updateValues(rooms, artifact_probability);

	int test_number_of_cycles = 1;
	while (continueValueIterating())
	{
		updateValues(rooms, artifact_probability);
		test_number_of_cycles++;
	}
	printf("Value Iteration Ended After %d Cycles.\n", test_number_of_cycles);
}

void ValueIteration::updateValues(vector<unique_ptr<Room>>& rooms, float artifact_probability)
{
	VI_previous = VI_current;
	VI_current.clear();
	m_difference = 0;
	
	//for each room in VI_current, the new value for that room should be
	// the max value of each of that room's neighbors in VI_previous (V)
	// times the discount factor (g)
	// plus the room's inherent reward (R)
	// new_value = R + g * V;
	
	for (unique_ptr<Room>& room : rooms)
	{
		float new_value;

		if (room->containsBoss())
		{
			new_value = 1.0;
		}
		else
		{

			float V = calculateHighestNeighborValue(room.get());

			new_value = calculateRoomReward(room.get(), artifact_probability) + DISCOUNT_FACTOR * V;
		}

		VI_current.emplace(room->getRoomID(), new_value);

		float old_value = VI_previous.at(room->getRoomID());

		if (abs(new_value - old_value) > m_difference)
		{
			m_difference = abs(new_value - old_value);
		}
		
		//for testing
		printf("The new value for room %d is %f \n", room->getRoomID(), new_value);
	}
}

Room::directions ValueIteration::getNextRoom(Room * current_room)
{
	float north_value = LOW_NUMBER_HACK;
	float south_value = LOW_NUMBER_HACK;
	float east_value = LOW_NUMBER_HACK;
	float west_value = LOW_NUMBER_HACK;

	// Load room values
	if (current_room->get_north_room() != nullptr)
	{
		north_value = VI_current.at(current_room->get_north_room()->getRoomID());
	}
	if (current_room->get_south_room() != nullptr)
	{
		south_value = VI_current.at(current_room->get_south_room()->getRoomID());
	}
	if (current_room->get_east_room() != nullptr)
	{
		east_value = VI_current.at(current_room->get_east_room()->getRoomID());
	}
	if (current_room->get_west_room() != nullptr)
	{
		west_value = VI_current.at(current_room->get_west_room()->getRoomID());
	}

	// Return best room
	if ((north_value != -100) && (north_value > south_value) && (north_value > east_value) && north_value > west_value)
	{
		//return current_room->get_north_room();
		return Room::directions::NORTH;
	}
	
	if ((south_value != -100) && (south_value > north_value) && (south_value > east_value) && south_value > west_value)
	{
		//return current_room->get_south_room();
		return Room::directions::SOUTH;
	}
	
	if ((east_value != -100) && (east_value > south_value) && (east_value > north_value) && east_value > west_value)
	{
		//return current_room->get_east_room();
		return Room::directions::EAST;
	}
	else
	{
		//return current_room->get_west_room();
		return Room::directions::WEST;
	}
	
}

float ValueIteration::calculateHighestNeighborValue(Room * room)
{
	vector<float> neighbor_values;

	if (room->get_north_room() !=	nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->get_north_room()->getRoomID()));
	}
	if (room->get_south_room() != nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->get_south_room()->getRoomID()));
	}
	if (room->get_east_room() != nullptr)
	{
		Room* eastRoom = room->get_east_room();
		auto it = VI_previous.find(eastRoom->getRoomID());
		auto pair = *it;
		float eastValue = pair.second;
		neighbor_values.push_back(eastValue);
	}
	if (room->get_west_room() != nullptr)
	{
		neighbor_values.push_back(VI_previous.at(room->get_west_room()->getRoomID()));
	}

	return *max_element(neighbor_values.begin(), neighbor_values.end());;
}

bool ValueIteration::continueValueIterating()
{
	float error = (MAX_ERROR * (1 - DISCOUNT_FACTOR) / DISCOUNT_FACTOR);
	return (m_difference >= error);
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

