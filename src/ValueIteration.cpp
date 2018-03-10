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

float ValueIteration::m_difference;
map<int, float> ValueIteration::VI_current;
map<int, float> ValueIteration::VI_previous;


Room::adjacent_room ValueIteration::getNextRoom(const Room * current_room, vector<unique_ptr<Room>>& rooms, float artifact_probability, Dungeon& dungeon)
{
	initialize(rooms, artifact_probability, dungeon);
	return getNextRoom(current_room, dungeon);
}

void ValueIteration::initialize(vector<unique_ptr<Room>>& rooms, float artifact_probability, Dungeon& dungeon)
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

	updateValues(rooms, artifact_probability, dungeon);

	int test_number_of_cycles = 1;
	while (continueValueIterating())
	{
		updateValues(rooms, artifact_probability, dungeon);
		test_number_of_cycles++;
	}
	printf("Value Iteration Ended After %d Cycles.\n", test_number_of_cycles);
}

void ValueIteration::updateValues(vector<unique_ptr<Room>>& rooms, float artifact_probability, Dungeon& dungeon)
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

			float V = calculateHighestNeighborValue(room.get(), dungeon);

			new_value = calculateRoomReward(room.get(), artifact_probability) + DISCOUNT_FACTOR * V;
		}

		VI_current.emplace(room->getRoomID(), new_value);

		float old_value = VI_previous.at(room->getRoomID());
		float value_difference = abs(new_value - old_value);
		
		if (value_difference > m_difference)
		{
			m_difference = value_difference;
		}
		
		//for testing
		printf("The new value for room %d is %f \n", room->getRoomID(), new_value);
	}
}

Room::adjacent_room ValueIteration::getNextRoom(const Room * current_room, Dungeon& dungeon)
{
  using room_value = std::pair<Room::adjacent_room, float>;
  
  struct CompValues
  {
    bool operator()(room_value& v1, room_value& v2)
    {
      return v1.second < v2.second;
    }
  };

  std::vector<room_value> values;
  const std::vector<Room::adjacent_room>& adjacents = dungeon.get_adjacent_rooms(current_room->getRoomID());

  for (const Room::adjacent_room& adj : adjacents)
  {
    values.push_back({ adj, VI_current.at(adj.room->getRoomID()) });
  }

  room_value max_value = *max_element(values.begin(), values.end(), CompValues());
  return max_value.first;	
}

float ValueIteration::calculateHighestNeighborValue(Room * room, Dungeon& dungeon)
{
	vector<float> neighbor_values;

  const vector<Room::adjacent_room>& adjacents = dungeon.get_adjacent_rooms(room->getRoomID());

  for (const Room::adjacent_room& adj : adjacents)
  {
    neighbor_values.push_back(VI_previous.at(adj.room->getRoomID()));
  }
	return *max_element(neighbor_values.begin(), neighbor_values.end());
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
	else if (room->containsUndiscoveredArtifact())
	{
		reward = ARTIFACT_VALUE * artifact_probability;
	}

	return reward;

}

