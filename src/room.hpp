#pragma once

#include "common.hpp"
#include "floor.hpp"
#include "wall.hpp"

#include <vector>

class Room : Renderable
{
public:
	Room();
	~Room();

	// Creates a room
	bool init();

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our room
	void draw(const mat3& projection, const mat3& parent_transform) override;

private:
	std::vector<Floor>		m_floors;
	std::vector<Wall>		m_walls;
}; 