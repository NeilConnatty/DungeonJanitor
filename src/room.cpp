// room.cpp

#include "room.hpp"

#define SPRITE_SIZE 64.f

Room::Room() {}

Room::~Room() {}

// Creates a room
bool Room::init(vec2 position) 
{
  m_position = position;
  m_scale = {2.f, 2.f};

  return true;
}

bool Room::init() { return init({0.f, 0.f}); }

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

  for (Puddle p : m_puddles)
  {
    p.destroy();
  }

  for (Door d : m_doors)
  {
    d.destroy();
  }
}

void Room::update_current(float ms) {}

void Room::update_children(float ms)
// at the moment the room's only children are walls and floors,
// which do not need to be updated
{}

void Room::draw_current(const mat3 &projection, const mat3 &current_transform) 
{
}

// Renders our room
void Room::draw_children(const mat3 &projection,
                         const mat3 &current_transform) 
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

  for (Door d : m_doors)
  {
    d.draw(projection, current_transform);
  }
}

bool Room::add_wall(wall_pair wall) 
{
  m_walls.emplace_back();
  if (!m_walls.back().init(wall.first, wall.second)) 
  {
    return false;
  }
  return true;
}

bool Room::add_floor(vec2 floor) 
{
  m_floors.emplace_back();
  if (!m_floors.back().init(floor)) 
  {
    return false;
  }
  return true;
}

bool Room::add_door(vec2 pos)
{
  m_doors.emplace_back();
  if (!m_doors.back().init(pos))
  {
    return false;
  }
  return true;
}

bool Room::add_floors(std::vector<vec2> &positions) 
{
  for (vec2 &pos : positions) 
  {
    if (!add_floor(pos)) 
    {
      return false;
    }
  }

  return true;
}

bool Room::add_doors(std::vector<vec2> &positions)
{
  for (vec2 &pos : positions)
  {
    if (!add_door(pos))
    {
      return false;
    }
  }

  return true;
}


bool Room::add_walls(std::vector<wall_pair> &walls) 
{
  for (wall_pair &pair : walls) 
  {
    if (!add_wall(pair)) 
    {
      return false;
    }
  }

  return true;
}

bool Room::add_cleanables(std::vector<vec2> &puddle_positions) 
{
  for (vec2 &pos : puddle_positions) 
  {
    m_puddles.emplace_back();
    if (!m_puddles.back().init(pos))
    {
      return false;
    }
    m_puddles.back().set_scale({ 0.5f, 0.5f });
  }
  return true;
}
int Room::get_num_cleanables() { return m_num_cleanables; }
float Room::get_clean_percent() { return (float)m_num_cleanables / (float)m_total_cleanables; }
void Room::decrement_cleanables() { m_num_cleanables--; }
std::vector<Puddle> &Room::get_cleanables() { return m_puddles; }

void Room::set_north_room(Room* rm)
{
  m_adjacent_rooms[NORTH] = rm;
}

void Room::set_south_room(Room* rm)
{
  m_adjacent_rooms[SOUTH] = rm;
}

void Room::set_east_room(Room* rm)
{
  m_adjacent_rooms[EAST] = rm;
}

void Room::set_west_room(Room* rm)
{
  m_adjacent_rooms[WEST] = rm;
}
