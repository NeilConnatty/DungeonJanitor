//pathfinder.cpp

#include <assert.h>
#include "pathfinder.hpp"

vector<vec2> Pathfinder::getPathFromPositionToDestination(vec2 position, vec2 destination, float x_speed, float y_speed)
{
	PathNode startNode = PathNode(position.x, position.y);
	PathNode endNode = PathNode(destination.x, destination.y);

	vector<PathNode> openNodes;
	vector<PathNode> closedNodes;

	openNodes.push_back(startNode);

	while (openNodes.size() > 0)
	{
		// choose next node_current from openNodes
		PathNode* node_current = getNextNode(&openNodes);

		// if node is match, done with pathfinding
		if (node_current->isMatch(endNode))
		{
			endNode.parent = node_current->parent;
			break;
		}


		// if collision -> close node and continue
		if (collisionDetected(*node_current))
		{
			closedNodes.push_back(*node_current);
			continue;
		}
		
		// else get successors and set their values
		vector<PathNode> successors; 
		node_current->getSuccessorNodes(&successors, &endNode, x_speed, y_speed);


		for (PathNode& successor_node : successors)
		{
			// see if in OPEN. new value should never be better
			
			auto found_node = find(openNodes.begin(), openNodes.end(), successor_node);
			if (found_node != openNodes.end())
			{
				if (found_node->getFValue() > successor_node.getFValue())
				{
					openNodes.erase(found_node);
					//Jay Should not get here. Previous node should always have more efficient path.
					//printf("Jay: Error - found more efficient node during A*");
				}
				else
				{
					// discard successor_node and continue for loop
					continue;
				}
			}

			// ditto CLOSED
			found_node = find(closedNodes.begin(), closedNodes.end(), successor_node);
			if (found_node != closedNodes.end())
			{
				if (found_node->getFValue() > successor_node.getFValue())
				{
					//Jay Should not get here. Previous node should always have more efficient path.
					//printf("Jay: Error - found more efficient node during A*");
					//assert(false);
					closedNodes.erase(found_node);
				}
				else
				{
					// discard successor_node and continue for loop
					continue;
				}
			}
			
			openNodes.push_back(successor_node);
			int test = 0;
		}
		closedNodes.push_back(*node_current);
	}
	return getPathFromGoalNode(endNode);
}

bool Pathfinder::collisionDetected(PathNode node)
{

	//stub
	return false;
}

PathNode* Pathfinder::getNextNode(vector<PathNode>* nodes)
{
	PathNode * bestSoFar = &(nodes->front());
	int count = 0;
	int finalCount = 0;

	for (PathNode& node : *nodes)
	{
		if (node.getFValue() < bestSoFar->getFValue())
		{
			bestSoFar = &node;
			finalCount = count;
		}
		count++;
	}

	nodes->erase(nodes->begin() + finalCount);

	return bestSoFar;
}

vector<vec2> Pathfinder::getPathFromGoalNode(PathNode endNode)
{
	vector<vec2> solutionPathList;
	PathNode* p = &endNode;
	
	while (p != nullptr)
	{
		vec2 nextNodePosition = vec2{ p->m_xCoord, p->m_yCoord };
		solutionPathList.emplace(solutionPathList.begin());
		p = p->parent;
	}

	return solutionPathList;
}


