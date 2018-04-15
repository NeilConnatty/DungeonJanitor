//pathfinder.cpp

#include <assert.h>
#include "pathfinder.hpp"
#include <memory>
#include <vector>
#include "dungeon.hpp"

void Pathfinder::getPathFromPositionToDestination(vec2 position, vec2 destination, float x_speed, float y_speed, 
	GameObject& moving_object, Room& room, vector<vec2>& path, Dungeon& dungeon)

{
	PathNode startNode = PathNode(position.x, position.y);
	PathNode endNode = PathNode(destination.x, destination.y);

	vector<unique_ptr<PathNode>> openNodes;
	vector<unique_ptr<PathNode>> closedNodes;

	openNodes.push_back(make_unique<PathNode>(startNode));

	while (openNodes.size() > 0)
	{
		// choose next node_current from openNodes
		std::unique_ptr<PathNode> node_current(getNextNode(&openNodes)); // moves the returned unique_ptr into node_current

		// if node is match, done with pathfinding
		if (node_current->isMatch(endNode))
		{
			endNode.parent = node_current->parent;
			break;
		}

		/*
		// if collision -> close node and continue
		if (collisionDetected(moving_object, room, *node_current, dungeon))

		{
			closedNodes.push_back(make_unique<PathNode>(*node_current));
			continue;
		}
		*/

		// else get successors and set their values
		vector<unique_ptr<PathNode>> successors;
		node_current->getSuccessorNodes(&successors, &endNode, x_speed, y_speed);

		// if we have seen this node before, ignore it
		for (unique_ptr<PathNode>& successor_node : successors)
		{
			if (nodeVisitedBefore(successor_node, openNodes, closedNodes))
			{
				continue;
			}

			// if collision -> close node and continue
			if (collisionDetected(moving_object, room, *successor_node, dungeon))

			{
				closedNodes.push_back(make_unique<PathNode>(*successor_node));
				continue;
			}

			openNodes.push_back(std::move(successor_node));
		}
		closedNodes.push_back(std::move(node_current));
	}

	getPathFromGoalNode(endNode, path);
}


bool Pathfinder::collisionDetected(GameObject& moving_object, Room& room, PathNode& node, Dungeon& dungeon)
{
	// To be updated when room has list of collidable objects

	vec2 node_pos = { node.m_xCoord, node.m_yCoord };

	if (room.getRoomID() != -1)
	{
		for (Wall& wall : room.get_walls())
		{
			if (moving_object.collides_with_projected(wall, node_pos, room.transform, room.getDungeonTransform()))
			{
				return true;
			}
		}

		for (auto& floor_object : room.get_floor_objects())
		{
			if (moving_object.collides_with_projected(*floor_object, node_pos, room.transform, room.getDungeonTransform()))
			{
				return true;
			}
		}
	}

	// Hallway Room
	else

	{
		// check all adjacent rooms
		for (auto &r : dungeon.get_adjacent_rooms(-1))
		{
			for (Wall& wall : r.room->get_walls())
			{
				if (moving_object.collides_with_projected(wall, node_pos, r.room->transform, r.room->getDungeonTransform()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Pathfinder::nodeVisitedBefore(unique_ptr<PathNode>& successor_node,
	vector<unique_ptr<PathNode>>& openNodes, vector<unique_ptr<PathNode>>& closedNodes)
{
	for (auto &node : openNodes)
	{
		if (*successor_node == *node)
		{
			return true;
		}
	}

	for (auto &node : closedNodes)
	{
		if (*successor_node == *node)
		{
			return true;
		}
	}
	
	return false;
}

unique_ptr<PathNode> Pathfinder::getNextNode(vector<unique_ptr<PathNode>>* nodes)
{
	PathNode * bestSoFar = &(*(nodes->front()));
	int count = 0;
	int finalCount = 0;

	for (unique_ptr<PathNode>& node : *nodes)
	{
		if (node->getFValue() < bestSoFar->getFValue())
		{
			bestSoFar = &(*node);
			finalCount = count;
		}
		count++;
	}

  // Need to make sure the node isn't deallocated on call to erase
  std::unique_ptr<PathNode> ret(std::move(*(nodes->begin() + finalCount)));
  nodes->erase(nodes->begin() + finalCount);

  return ret;
}

// Returns path in reverse order. Pop_back to get the next position to go to.
void Pathfinder::getPathFromGoalNode(PathNode endNode, vector<vec2>& solutionPathList)
{
	PathNode* p = &endNode;
	
	while (p != nullptr)
	{
		vec2 nextNodePosition = vec2{ p->m_xCoord, p->m_yCoord };
		solutionPathList.emplace_back(nextNodePosition);
		p = p->parent;
	}
}
