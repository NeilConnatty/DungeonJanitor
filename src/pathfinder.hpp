#pragma once


#include "common.hpp"
#include "room.hpp"
#include "pathfindernode.hpp"
#include <vector>

using namespace std;


class Pathfinder
{
public:

	static vector<vec2> getPathFromPositionToDestination(vec2 position, vec2 destination);

private:

	static bool collisionDetected(PathNode node);

	static vector<vec2> getPathFromGoalNode(PathNode endNode);

	static void updateNodeListValuesWithSuccessor(vector<PathNode> node_list, PathNode successor_node);
};
