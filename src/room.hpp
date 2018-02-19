#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "Cleanable/puddle.hpp"

#include <vector>
#include <memory>

using namespace std;

class Room : public GameObject
{
public: 
  using wall_pair = pair<vec2, wall_edge>;

public:
	Room();
	~Room();

	bool init();
	bool init(vec2 position);
	void destroy();

  bool add_wall(wall_pair wall);
  bool add_floor(vec2 floor);
	bool add_walls(vector<wall_pair>& walls);
	bool add_floors(vector<vec2>& positions);
	bool add_cleanables(vector<vec2>& puddle_positions);
	bool add_boss(vec2 boss_position);
	bool add_hero(vec2 hero_position);

	vector<Puddle>&  get_cleanables();

private:
	void update_current(float ms) override;
	void update_children(float ms) override;
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
	vector<Floor>		m_floors;
	vector<Wall>		  m_walls;
	vector<Puddle>   m_puddles;
}; 
