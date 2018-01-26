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

	bool init();
	bool init(vec2 position);
	void destroy();

	void set_position(vec2 pos);
	bool update(float ms);
	void draw(const mat3& projection, const mat3& parent_transform) override;

private:
	std::vector<Floor>		m_floors;
	std::vector<Wall>		m_walls;

	vec2	m_position;
	vec2	m_scale;
}; 
