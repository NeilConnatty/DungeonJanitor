#pragma once

#include <math.h>

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

	float getFValue() { return G + H; }
	float getManhattanDistance(PathNode * destinationNode) 
	{
		float x = fabs(m_xCoord - destinationNode->m_xCoord);
		float y = fabs(m_yCoord - destinationNode->m_yCoord);
		return x + y;
	}
	bool isMatch(PathNode endNode);
	
};