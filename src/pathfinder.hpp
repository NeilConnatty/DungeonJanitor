#pragma once


#include "common.hpp"
#include "room.hpp"
#include <vector>

using namespace std;


class Pathfinder
{
public:

	static vector<vec2> getPathFromPositionToDestination(vec2 position, vec2 destination);

private:

	bool collisionDetected(vec2 position);
};
