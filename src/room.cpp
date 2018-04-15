// room.cpp

#include "room.hpp"
#include "janitor.hpp"
#include "desk.hpp"
#include "bathroomstall.hpp"

#define SPRITE_SIZE 64.f

Room::Room() {}

Room::~Room() {}

// Creates a room
bool Room::init(vec2 position, room_type type) 
{
  m_position = position;
  m_scale = {2.f, 2.f};
  m_has_boss_spawn_loc = false;
  m_has_hero_spawn_loc = false;
  m_has_janitor_spawn_loc = false;
  m_BossHere = false;
  m_ArtifactHere = false;
  m_room_type = type;
  m_num_cleaned_cleanables = 0;
  m_total_cleanables = 0;
  m_num_activated_artifacts = 0;
  m_total_artifacts = 0;
  m_hero_has_visited = false;
  m_num_spawned_boss_fight_cleanables = 0;
  m_num_cleaned_boss_fight_cleanables = 0;
  return true;
}

bool Room::init() { return init({0.f, 0.f}, HALLWAY_ROOM); }

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
  
  for (unique_ptr<Cleanable>& c : m_cleanables)
  {
	  c->destroy();
  }

  for (unique_ptr<FloorObject>& fo : m_floor_objects)
  {
    fo->destroy();
  }

  if (m_ArtifactHere)
  {
	  m_artifact.destroy();
  }

}

void Room::set_room_type(room_type type)
{
  m_room_type = type;
}

bool Room::populate_floor_objects()
{
  if (m_room_type == OFFICE_ROOM)
  {
    m_floor_objects.emplace_back(new Desk);
    if (!m_floor_objects.back()->init(m_floors[1].get_pos()))
    {
      fprintf(stderr, "failed to init large desk object.\n");
      return false;
    }
  }
  else if (m_room_type == CLASS_ROOM)
  {
    struct init_desk
    {
      bool operator()(Room* room, size_t i)
      {
        Desk* desk = new Desk();
        if (!desk->init(room->m_floors[i].get_pos(), true))
        {
          fprintf(stderr, "failed to init small desk object.\n");
          delete desk;
          return false;
        }
        room->m_floor_objects.emplace_back(desk);
        return true;
      }
    };

    init_desk initter;
    if (!(initter(this, 8) && initter(this, 10) && initter(this, 12) &&
          initter(this, 15) && initter(this, 17) && initter(this, 19) &&
          initter(this, 22) && initter(this, 24) && initter(this, 26))) 
    {
      return false;
    }
  }
  else if (m_room_type == BATH_ROOM)
  {
    struct init_stall
    {
      bool operator()(Room* room, size_t i)
      {
        BathroomStall* stall = new BathroomStall;
        if (!stall->init({ room->m_floors[i].get_pos().x, room->m_floors[i].get_pos().y - 12.f }))
        {
          fprintf(stderr, "failed to init bath stall object.\n");
          delete stall;
          return false;
        }
        room->m_floor_objects.emplace_back(stall);
        return true;
      }
    };

    init_stall initter;
    if (!(initter(this, 0) && initter(this, 1) && initter(this, 2))) 
    {
      return false;
    }
  }

  return true;
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

  for (unique_ptr<Cleanable>& c : m_cleanables)
  {
    c->draw(projection, current_transform);
  }

  for (unique_ptr<FloorObject>& fo : m_floor_objects)
  {
    fo->draw(projection, current_transform);
  }

  if (m_ArtifactHere)
  {
		m_artifact.draw(projection, current_transform);

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
  if (!m_floors.back().init(floor, m_room_type)) 
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

bool Room::add_cleanables(vector<pair<Cleanable::types, vec2>>& cleanable_pos)
{
	if (containsBoss())
	{
		for (Floor &floor : m_floors)
		{
			Puddle* p = new Puddle();
			if (!p->init(floor.get_pos()))
			{
				return false;
			}
			m_cleanables.emplace_back(p);
			p->toggle_enable();
		}
	}
	else if (!cleanable_pos.empty())
	{
		for (pair<Cleanable::types, vec2>& cleanable : cleanable_pos)
		{
			m_total_cleanables++;
			if (cleanable.first == Cleanable::types::PUDDLE)
			{
				Puddle* p = new Puddle();
				if (!p->init(cleanable.second))
				{
					return false;
				}
				m_cleanables.emplace_back(p);
			}
			else if (cleanable.first == Cleanable::types::GRAFFITI)
			{
				Graffiti* g = new Graffiti();
				if (!g->init(cleanable.second))
				{
					return false;
				}
				m_cleanables.emplace_back(g);
			}
			else if (cleanable.first == Cleanable::types::GARBAGE)
			{
				Garbage* g = new Garbage();
				if (!g->init(cleanable.second))
				{
					return false;
				}
				m_cleanables.emplace_back(g);
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
		m_total_artifacts++;
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

void Room::clean(Janitor* janitor, mat3 dungeon_transform)
{
	for (unique_ptr<Cleanable>& c : get_cleanables()) {
		if (c->is_enabled() && janitor->collides_with(*c, this->transform, dungeon_transform)) {
			if (c.get()->clean())
			{
				c->play_sound();
				if (containsBoss())
				{
					m_num_cleaned_boss_fight_cleanables++;
				}
				else
				{
					increment_cleaned_cleanables();
				}
			}
		}
	}

	if (containsArtifact())
	{
		if (janitor->collides_with(*get_artifact(), this->transform, dungeon_transform))
		{
			if (!get_artifact()->is_activated())
			{
				get_artifact()->set_active(true);
				set_hero_has_visited(false);
				increment_activated_artifacts();
			}
		}
	}
}

vector<Wall> &Room::get_walls() { return m_walls; }
vector<unique_ptr<Cleanable>> &Room::get_cleanables() { return m_cleanables; }
vector<unique_ptr<FloorObject>>& Room::get_floor_objects() { return m_floor_objects; }

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

bool Room::containsArtifact()
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
}

Artifact* Room::get_artifact()
{
	return &m_artifact;
}

void Room::setRoomID(int id)
{
  m_ID = id;
}

void Room::set_hero_has_visited(bool visited)
{
	m_hero_has_visited = visited;
}

bool Room::has_hero_visited()
{
	return m_hero_has_visited;
}

void Room::spawn_debris()
{
	default_random_engine rng;
	uniform_int_distribution<int> dist;
	rng = default_random_engine(random_device()());
	dist = uniform_int_distribution<int>(0, 25);
	int random = dist(rng);
	if (random == 1)
	{
		dist = uniform_int_distribution<int>(0, m_cleanables.size() - 1);
		unique_ptr<Cleanable>& c = m_cleanables.at(dist(rng));
		if (!c->is_enabled())
		{
			m_num_spawned_boss_fight_cleanables++;
			c->toggle_enable();
		}
	}
}
