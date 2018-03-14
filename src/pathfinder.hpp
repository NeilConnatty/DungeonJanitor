#pragma once


#include "common.hpp"
#include "room.hpp"
#include "pathfindernode.hpp"
#include <vector>
#include <memory>

using namespace std;


class Pathfinder
{
public:

	static void getPathFromPositionToDestination(vec2 position, vec2 destination, float x_speed, float y_speed, vector<vec2>& path);

private:

	static bool collisionDetected(PathNode& node);
	static unique_ptr<PathNode> getNextNode(vector<unique_ptr<PathNode>>* nodes);
	static void getPathFromGoalNode(PathNode endNode, vector<vec2>& path);
	
};
