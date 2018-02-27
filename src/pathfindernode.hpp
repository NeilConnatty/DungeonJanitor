#pragma once
#include "common.hpp"
#include <math.h>
#include <vector>

using namespace std;


struct PathNode
{


public:
	float m_xCoord, m_yCoord;
	int m_id;
	PathNode * parent;
	float G, H;

	PathNode()
	{
		parent = nullptr;
	}

	PathNode(float x, float y)
	{
		m_xCoord = x;
		m_yCoord = y;

		//m_id =  need to set id

		parent = nullptr;
		G = 0;
		H = 0;

	}

	bool isMatch(PathNode endNode);

	bool operator==(const PathNode& comparison_node);
	
	float getFValue() { return G + H; };
	float getManhattanDistance(PathNode * destinationNode);
	vector<PathNode> getSuccessorNodes(PathNode* endNode);
	
	
};