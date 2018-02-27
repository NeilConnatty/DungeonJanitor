// pathfindernode.cpp

#include "pathfindernode.hpp";

#define CLOSE_ENOUGH 0.01f; // Subject to change

bool PathNode::isMatch(PathNode endNode)
{
	return (*this == endNode);
}

bool PathNode::operator==(const PathNode & comparison_node)
{
	bool c1 = abs(this->m_xCoord - comparison_node.m_xCoord) <= CLOSE_ENOUGH;
	bool c2 = abs(this->m_yCoord - comparison_node.m_yCoord) <= CLOSE_ENOUGH;
	return c1 && c2;
	
}

float PathNode::getManhattanDistance(PathNode * destinationNode)
{
	float x = fabs(m_xCoord - destinationNode->m_xCoord);
	float y = fabs(m_yCoord - destinationNode->m_yCoord);
	return x + y;
}

vector<PathNode> PathNode::getSuccessorNodes(PathNode* endNode)
{
	vector<PathNode> successor_nodes;
	
	float x_speed, y_speed; // should be set earlier
	float diagonal = sqrt(pow(x_speed, 2) + pow(y_speed, 2));

	PathNode pn1 = PathNode(this->m_xCoord + x_speed, this->m_yCoord);
	PathNode pn2 = PathNode(this->m_xCoord - x_speed, this->m_yCoord);
	PathNode pn3 = PathNode(this->m_xCoord + x_speed, this->m_yCoord + y_speed);
	PathNode pn4 = PathNode(this->m_xCoord + x_speed, this->m_yCoord - y_speed);
	PathNode pn5 = PathNode(this->m_xCoord - x_speed, this->m_yCoord + y_speed);
	PathNode pn6 = PathNode(this->m_xCoord - x_speed, this->m_yCoord - y_speed);
	PathNode pn7 = PathNode(this->m_xCoord			, this->m_yCoord + y_speed);
	PathNode pn8 = PathNode(this->m_xCoord			, this->m_yCoord - y_speed);

	// Set G based on distance from this node
	pn1.G = this->G + x_speed;
	pn2.G = this->G + x_speed;
	pn3.G = this->G + diagonal;
	pn4.G = this->G + diagonal;
	pn5.G = this->G + diagonal;
	pn6.G = this->G + diagonal;
	pn7.G = this->G + y_speed;
	pn8.G = this->G + y_speed;

	successor_nodes.emplace_back(pn1);
	successor_nodes.emplace_back(pn2);
	successor_nodes.emplace_back(pn3);
	successor_nodes.emplace_back(pn4);
	successor_nodes.emplace_back(pn5);
	successor_nodes.emplace_back(pn6);
	successor_nodes.emplace_back(pn7);
	successor_nodes.emplace_back(pn8);

	for (PathNode node : successor_nodes)
	{
		node.parent = this;
		node.H = node.getManhattanDistance(endNode);
	}

	return successor_nodes;
}
