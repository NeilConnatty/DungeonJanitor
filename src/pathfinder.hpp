#pragma once


#include "common.hpp"
#include "room.hpp"
#include "dungeon.hpp"
#include "pathfindernode.hpp"
#include <vector>
#include <memory>

class Dungeon;
using namespace std;


class Pathfinder
{
public:

	static void getPathFromPositionToDestination(vec2 position, vec2 destination, float x_speed, float y_speed,
		GameObject& moving_object, Room& room, vector<vec2>& path, Dungeon& dungeon);

private:

	static bool collisionDetected(GameObject& moving_object, Room& room, PathNode& node, Dungeon& dungeon);

	static unique_ptr<PathNode> getNextNode(vector<unique_ptr<PathNode>>* nodes);
	static void getPathFromGoalNode(PathNode endNode, vector<vec2>& path);
	
};
