#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "Cleanable/puddle.hpp"
#include "janitor.hpp"

#include <vector>
#include <memory>

class Room : public GameObject
{
public: 
	using ptr = std::unique_ptr<Room>;

public:
	Room();
	~Room();

	bool init();
	bool init(vec2 position);
	void destroy();

	bool add_walls(std::vector<vec2>& positions);
	bool add_floors(std::vector<vec2>& positions);
	bool add_cleanables(std::vector<vec2>& puddle_positions);
	bool handle_collision(Janitor player);

private:
	void update_current(float ms) override;
	void update_children(float ms) override;
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
	std::vector<Floor>		m_floors;
	std::vector<Wall>		m_walls;
	std::vector<Puddle>     m_puddles;
}; 
