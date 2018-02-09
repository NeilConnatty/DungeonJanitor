// room.cpp

#include "room.hpp"

#define SPRITE_SIZE 64.f

Room::Room()
{
}

Room::~Room()
{
}

// Creates a room
bool Room::init(vec2 position)
{
	m_position = position;
	m_scale = { 2.f, 2.f };

	return true;
}

bool Room::init()
{
	return init({ 0.f, 0.f });
}

// Releases all associated resources
void Room::destroy()
{
	for (Floor f : m_floors)
	{
		f.destroy();
	}

	for (Wall w : m_walls)
	{
		w.destroy();
	}
}

void Room::update_current(float ms)
{
}

void Room::update_children(float ms)
// at the moment the room's only children are walls and floors,
// which do not need to be updated
{}

void Room::draw_current(const mat3& projection, const mat3& current_transform)
{
}

// Renders our room
void Room::draw_children(const mat3& projection, const mat3& current_transform)
{

	for (Floor f : m_floors)
	{
		f.draw(projection, current_transform);
	}

	for (Wall w : m_walls)
	{
		w.draw(projection, current_transform);
	}
	
	for (Puddle p : m_puddles)
	{
		p.draw(projection, current_transform);
	}
}

bool Room::add_wall(wall_pair wall)
{
  m_walls.emplace_back();
  if (!m_walls.back().init(wall.first, wall.second))
  {
    return false;
  }
}

bool Room::add_floor(vec2 floor)
{
  m_floors.emplace_back();
  if (!m_floors.back().init(floor))
  {
    return false;
  }
}

bool Room::add_floors(std::vector<vec2>& positions)
{
	for (vec2& pos : positions)
	{
    if (!add_floor(pos))
    {
      return false;
    }
	}

	return true;
}

bool Room::add_walls(std::vector<wall_pair>& walls)
{
	for (wall_pair& pair : walls)
	{
    if (!add_wall(pair))
    {
      return false;
    }
	}

	return true;
}

bool Room::add_cleanables(std::vector<vec2>& puddle_positions)
{
	for (vec2& pos : puddle_positions)
	{
		m_puddles.emplace_back();
		if (!m_puddles.back().init(pos))

		{
			return false;
		}
	}
	return true;
}

std::vector<Puddle>& Room::get_cleanables()
{
	return m_puddles;
}
