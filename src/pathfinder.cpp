//pathfinder.cpp

#include "pathfinder.hpp"

vector<vec2> Pathfinder::getPathFromPositionToDestination(vec2 position, vec2 destination)
{
	PathNode startNode = PathNode(position.x, position.y);
	PathNode endNode = PathNode(destination.x, destination.y);

	vector<PathNode> openNodes;
	vector<PathNode> closedNodes;

	openNodes.push_back(startNode);

	while (openNodes.size() > 0)
	{

		PathNode node_current;

		if (node_current.isMatch(endNode))
		{
			endNode.parent = node_current.parent;
			break;
		}

		//else do stuff!

	}


	return getPathFromGoalNode(endNode);
}

bool Pathfinder::collisionDetected(vec2 position)
{

	//stub
	return false;
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

