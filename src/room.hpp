#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "Cleanable/puddle.hpp"

#include <vector>
#include <memory>

class Room : public GameObject
{
public: 
  using wall_pair = std::pair<vec2, wall_edge>;

public:
	Room();
	~Room();

	bool init();
	bool init(vec2 position);
	void destroy();
	
	bool add_wall(wall_pair wall);
	bool add_floor(vec2 floor);
	bool add_walls(std::vector<wall_pair>& walls);
	bool add_floors(std::vector<vec2>& positions);
	bool add_cleanables(std::vector<vec2>& puddle_positions);

	std::vector<Puddle>&  get_cleanables();
	int get_num_cleanables();
	float get_clean_percent();
	void decrement_cleanables();


private:
	void update_current(float ms) override;
	void update_children(float ms) override;
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
	int m_num_cleanables;
	int m_total_cleanables;
	std::vector<Floor>		m_floors;
	std::vector<Wall>		  m_walls;
	std::vector<Puddle>   m_puddles;
}; 
