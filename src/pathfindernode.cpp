// pathfindernode.cpp

#include "pathfindernode.hpp"
#include <memory>

#define DIAGONAL_ALLOWED false
#define CLOSE_ENOUGH 0.001f // Subject to change
#define IS_MATCH 10.f // Subject to change

bool PathNode::isMatch(PathNode endNode)
{
	bool c1 = abs(this->m_xCoord - endNode.m_xCoord) <= IS_MATCH;
	bool c2 = abs(this->m_yCoord - endNode.m_yCoord) <= IS_MATCH;
	return c1 && c2;
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

void PathNode::getSuccessorNodes(vector<unique_ptr<PathNode>> * successor_nodes, PathNode* endNode, float x_speed, float y_speed)
{
	float diagonal = sqrt(pow(x_speed, 2) + pow(y_speed, 2));

  if (m_xCoord < 0 || m_yCoord < 0)
  {
    fprintf(stderr, "Arrived at an invalid part of Path-finding code in "
                    "PathNode::getSuccessorNodes(). Check with Jay.\n");
  }

	PathNode* pn1 = new PathNode(this->m_xCoord + x_speed, this->m_yCoord);
	PathNode* pn2 = new PathNode(this->m_xCoord - x_speed, this->m_yCoord);
	PathNode* pn7 = new PathNode(this->m_xCoord			, this->m_yCoord + y_speed);
	PathNode* pn8 = new PathNode(this->m_xCoord			, this->m_yCoord - y_speed);

	// Set G based on distance from this node
	pn1->G = this->G + x_speed;
	pn2->G = this->G + x_speed;
	pn7->G = this->G + y_speed;
	pn8->G = this->G + y_speed;

	successor_nodes->emplace_back(pn1);
	successor_nodes->emplace_back(pn2);
	successor_nodes->emplace_back(pn7);
	successor_nodes->emplace_back(pn8);

	if (DIAGONAL_ALLOWED)
	{
		PathNode* pn3 = new PathNode(this->m_xCoord + x_speed, this->m_yCoord + y_speed);
		PathNode* pn4 = new PathNode(this->m_xCoord + x_speed, this->m_yCoord - y_speed);
		PathNode* pn5 = new PathNode(this->m_xCoord - x_speed, this->m_yCoord + y_speed);
		PathNode* pn6 = new PathNode(this->m_xCoord - x_speed, this->m_yCoord - y_speed);

		pn3->G = this->G + diagonal;
		pn4->G = this->G + diagonal;
		pn5->G = this->G + diagonal;
		pn6->G = this->G + diagonal;

		successor_nodes->emplace_back(pn3);
		successor_nodes->emplace_back(pn4);
		successor_nodes->emplace_back(pn5);
		successor_nodes->emplace_back(pn6);
	}

	for (std::unique_ptr<PathNode>& node : *successor_nodes)
	{
		node->parent = this;
		node->H = node->getManhattanDistance(endNode);
	}

}
