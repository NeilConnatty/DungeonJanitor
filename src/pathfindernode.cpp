// pathfindernode.cpp

#include "pathfindernode.hpp";

bool PathNode::isMatch(PathNode endNode)
{
	// stub
	// Needs to check distance within some acceptable range
	return false;
}

float PathNode::getManhattanDistance(PathNode * destinationNode)
{
	float x = fabs(m_xCoord - destinationNode->m_xCoord);
	float y = fabs(m_yCoord - destinationNode->m_yCoord);
	return x + y;
}

vector<PathNode> PathNode::getSuccessorNodes()
{
	//stub
	// Should generate all successor nodes and set their values!

	return vector<PathNode>();
}
