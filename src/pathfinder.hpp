#pragma once


#include "common.hpp"
#include "room.hpp"
#include "pathfindernode.hpp"
#include <vector>

using namespace std;


class Pathfinder
{
public:

	static vector<vec2> getPathFromPositionToDestination(vec2 position, vec2 destination, float x_speed, float y_speed);

private:

	static bool collisionDetected(PathNode node);
	static PathNode* getNextNode(vector<PathNode>* nodes);
	static vector<vec2> getPathFromGoalNode(PathNode endNode);
};
