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
  m_has_boss_spawn_loc = false;
  m_has_hero_spawn_loc = false;
  m_has_janitor_spawn_loc = false;
  m_BossHere = false;
  m_has_door = false;
  m_ArtifactHere = false;

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

  m_door.destroy();
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

  if (m_ArtifactHere)
  {
		m_artifact.draw(projection, current_transform);

  }

  if (m_has_door)
  {
    m_door.draw(projection, current_transform);
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
  if (m_has_door)
  {
    m_door.set_pos({ pos.x, pos.y + 25.f });
    return true;
  }

  if (!m_door.init({ pos.x, pos.y + 18.f }))
  {
    return false;
  }
  m_has_door = true;

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
    if (!puddle_positions.empty()) {
        for (vec2 &pos : puddle_positions)
        {
            m_puddles.emplace_back();
            if (!m_puddles.back().init(pos))
            {
                return false;
            }
        }
    }
  return true;
}


bool Room::add_artifact(bool has_artifact, vec2 artifact_pos)
{

	m_ArtifactHere = has_artifact;
    if (has_artifact)
    {
        if (!m_artifact.init(artifact_pos))
        {
            return false;
        }
        m_artifact.set_scale({ 0.5f, 0.5f });
    }
    return true;
}

bool Room::add_hero_spawn_loc(bool has_hero_spawn_loc, vec2 hero_spawn_loc)
{
	m_has_hero_spawn_loc = has_hero_spawn_loc;
	m_hero_spawn_loc = hero_spawn_loc;
	return true;
}

bool Room::add_boss_spawn_loc(bool has_boss_spawn_loc, vec2 boss_spawn_loc)
{

	m_has_boss_spawn_loc = has_boss_spawn_loc;
	m_boss_spawn_loc = boss_spawn_loc;
	m_BossHere = has_boss_spawn_loc;
	return true;
}

bool Room::add_janitor_spawn_loc(bool has_janitor_spawn_loc, vec2 janitor_spawn_loc)
{
	m_has_janitor_spawn_loc = has_janitor_spawn_loc;
	m_janitor_spawn_loc = janitor_spawn_loc;
	return true;
}

//int Room::get_num_cleanables() { return m_num_cleanables; }
//float Room::get_clean_percent() { return (float)m_num_cleanables / (float)m_total_cleanables; }
//void Room::decrement_cleanables() { m_num_cleanables--; }

std::vector<Wall> &Room::get_walls() { return m_walls; }

std::vector<Puddle> &Room::get_cleanables() { return m_puddles; }
int Room::get_num_cleanables() { return m_num_cleanables; }
float Room::get_clean_percent() { return (float)m_num_cleanables / (float)m_total_cleanables; }
void Room::decrement_cleanables() { m_num_cleanables--; }

double Room::getReward() const 
{
    return heroRewardValue;
}

void Room::setReward(double reward)
{
    heroRewardValue = reward;
}

bool Room::has_hero_spawn_loc() const
{
    return m_has_hero_spawn_loc;
}

vec2 Room::get_hero_spawn_loc() const
{
    if (!m_has_hero_spawn_loc)
    {
        fprintf(stderr, "Hero doesn't spawn here");
        return { -1, -1 };
    }
    return m_hero_spawn_loc;
}

bool Room::has_boss_spawn_loc() const
{
    return m_has_boss_spawn_loc;
}

vec2 Room::get_boss_spawn_loc() const
{
    if (!m_has_boss_spawn_loc)
    {
        fprintf(stderr, "Boss doesn't spawn here");
        return { -1, -1 };
    }
    return m_boss_spawn_loc;
}

bool Room::has_janitor_spawn_loc() const
{
    return m_has_janitor_spawn_loc;
}

vec2 Room::get_janitor_spawn_loc() const
{
    if (!m_has_janitor_spawn_loc)
    {
        fprintf(stderr, "Janitor doesn't spawn here" );
        return { -1, -1 };
    }
    return m_janitor_spawn_loc;
}

// If boss is currently in the room
bool Room::containsBoss() const
{
    return m_BossHere;
}

bool Room::containsUndiscoveredArtifact()
{
	return m_ArtifactHere;
}

void Room::setBossInRoom(bool bossInRoom)
{
    m_BossHere = bossInRoom;
}

void Room::deactivate_artifact()
{
	m_artifact.set_active(false);
	m_ArtifactHere = false; // removes it from scene - if we want to have a different asset for artifacts the boss has interacted with we will need to get rid of this
}

Artifact* Room::get_artifact()
{
	return &m_artifact;
}

void Room::setRoomID(int id)
{
  m_ID = id;
}

void Room::add_adjacent_room(adjacent_room rm)
{
  m_adjacent_rooms.push_back(rm);
}

void Room::add_adjacent_room(Room* rm, Door* d)
{
  add_adjacent_room({ rm, d });
}
