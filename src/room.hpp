#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "door.hpp"
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
  bool add_floor(vec2 pos);
  bool add_door(vec2 pod);
	bool add_walls(std::vector<wall_pair>& walls);
  bool add_floors(std::vector<vec2>& positions);
  bool add_doors(std::vector<vec2>& positions);
	bool add_cleanables(std::vector<vec2>& puddle_positions);

	std::vector<Puddle>&  get_cleanables();

private:
	void update_current(float ms) override;
	void update_children(float ms) override;
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
	std::vector<Floor>		m_floors;
	std::vector<Wall>		  m_walls;
	std::vector<Puddle>   m_puddles;
  std::vector<Door>     m_doors;
 }; 
