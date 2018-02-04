#pragma once

#include "common.hpp"
#include "floor.hpp"
#include "wall.hpp"

#include <vector>
#include <memory>

class Room : Renderable
{
public: 
	using ptr = std::unique_ptr<Room>;

public:
	Room();
	~Room();

	bool init();
	bool init(vec2 position);
	void destroy();

	void set_position(vec2 pos);
	bool update(float ms);
	void draw(const mat3& projection, const mat3& parent_transform) override;

	bool add_walls(std::vector<vec2>& positions);
	bool add_floors(std::vector<vec2>& positions);

private:
	std::vector<Floor>		m_floors;
	std::vector<Wall>		m_walls;

	vec2	m_position;
	vec2	m_scale;
}; 
