// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"
#include "ValueIteration.hpp" // testing
#include <iostream>

#define ARTIFACT_VALUE 5
#define HERO_TIME_TO_SPAWN 5000.f
#define BOSS_FIGHT_TIME 30000.f

Dungeon::Dungeon() : 
    GameObject()
{
}

Dungeon::~Dungeon() {}

bool Dungeon::init() 
{
  DungeonParser parser;
  if (!parser.parseDungeon(m_rooms, dungeon_path("1.dn"), *this))
  {
    fprintf(stderr, "Failed to parse dungeon");
    return false;
  }

  for (unique_ptr<Room> &room : m_rooms) 
  {
    if (room->has_janitor_spawn_loc()) 
    {
      janitor_start_room = room.get();
      janitor_room_position = room->get_janitor_spawn_loc();
    }

    if (room->has_hero_spawn_loc()) 
    {
      hero_start_room = room.get();
      hero_room_position = room->get_hero_spawn_loc();
    }

    if (room->has_boss_spawn_loc()) 
    {
      boss_start_room = room.get();
      boss_room_position = room->get_boss_spawn_loc();
    }
  }
  //m_emitters.emplace_back();
  vec2 velocity = {10.0f, 1.0f};
  vec4 color = {1.0f, 0.0f, 0.0f, 1.0f};
  //m_emitters.back().init(janitor_room_position, velocity, color, 100.0f, 30);
  m_hero_timer = HERO_TIME_TO_SPAWN; // Three minutes in milliseconds
  m_boss_fight_timer = BOSS_FIGHT_TIME;
  m_should_spawn_hero = false;
  m_boss_fight_has_ended = false;
  m_hero_has_spawned = false;
  m_boss_fight_has_started = false;
  return true;
}

void Dungeon::destroy()
{
	m_adjacency_map.clear();
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->destroy();
	}
	for (std::unique_ptr<Door>& door : m_doors)
	{
		door->destroy();
	}
	for (int i = 0; i < m_emitters.size(); ++i)
	{
		m_emitters[i].destroy();
	}
	m_healthBar = NULL;
	m_boss->destroy();
}

vector<unique_ptr<Room>>& Dungeon::get_rooms()
{
	return m_rooms;
}

void Dungeon::update_current(float ms)
{
	if (!m_hero_has_spawned)
	{
		m_hero_timer -= ms;

		if (m_hero_timer < 0)
		{
			m_should_spawn_hero = true;
		}
	}
	if (m_boss_fight_has_started)
	{
		m_boss_fight_timer -= ms;
		if (m_boss_fight_timer < 0)
		{
			m_boss_fight_has_ended = true;
		}
	}
}

void Dungeon::update_children(float ms)
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->update(ms);
	}

  for (std::unique_ptr<Door>& door : m_doors)
  {
    door->update(ms);
  }
  	for (int i = 0; i < m_emitters.size(); ++i)
	{
		m_emitters[i].update(ms);
	}
	if (has_boss_fight_started())
	{
		m_healthBar->set_percent_filled(get_boss_fight_dungeon_health());
	}
	else
	{
		m_healthBar->set_percent_filled(get_percent_dungeon_cleaned());
	}
}

void Dungeon::draw_current(const mat3& projection, const mat3& current_transform)
{

}

void Dungeon::draw_children(const mat3& projection, const mat3& current_transform)
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->draw(projection, current_transform);
	}

  for (std::unique_ptr<Door>& door : m_doors)
  {
    door->draw(projection, current_transform);
  }
    	for (int i = 0; i < m_emitters.size(); ++i)
	{
		m_emitters[i].draw(projection, current_transform);
	}
}

float Dungeon::get_percent_dungeon_cleaned()
{
	float cleaned_cleanables = 0;
	float total_cleanables = 0;
	float activated_artifacts = 0;
	float total_artifacts = 0;

	for (std::unique_ptr<Room>& room : m_rooms)
	{
		cleaned_cleanables = cleaned_cleanables + room->get_number_cleaned_cleanables();
		total_cleanables = total_cleanables + room->get_number_total_cleanables();
		activated_artifacts = activated_artifacts + room->get_number_activated_artifacts();
		total_artifacts = total_artifacts + room->get_number_total_artifacts();
	}
	
	return (cleaned_cleanables + activated_artifacts * ARTIFACT_VALUE) / (total_cleanables + total_artifacts * ARTIFACT_VALUE);
}

float Dungeon::get_boss_fight_dungeon_health()
{
	float cleaned_cleanables = 0;
	float total_cleanables = 0;
	float activated_artifacts = 0;
	float total_artifacts = 0;
	float spawned_boss_cleanables = 0;
	float cleaned_boss_cleanables = 0;
	float BOSS_SPAWN_VALUE = 0.03;
	float BOSS_CLEANABLE_VALUE = 0.02;

	for (std::unique_ptr<Room>& room : m_rooms)
	{
		cleaned_cleanables = cleaned_cleanables + room->get_number_cleaned_cleanables();
		total_cleanables = total_cleanables + room->get_number_total_cleanables();
		activated_artifacts = activated_artifacts + room->get_number_activated_artifacts();
		total_artifacts = total_artifacts + room->get_number_total_artifacts();
		if (room->containsBoss())
		{
			spawned_boss_cleanables = room->get_number_spawned_boss_cleanables();
			cleaned_boss_cleanables = room->get_number_cleaned_boss_cleanables();
		}
	}
	float cleaned_percent = (cleaned_cleanables + activated_artifacts * ARTIFACT_VALUE) / (total_cleanables + total_artifacts * ARTIFACT_VALUE);
	cleaned_percent = cleaned_percent + BOSS_CLEANABLE_VALUE * cleaned_boss_cleanables - BOSS_SPAWN_VALUE * spawned_boss_cleanables;
	cleaned_percent = max(cleaned_percent, 0.0f);
	return min(1.00f, cleaned_percent);
}

bool Dungeon::add_doors(vector<std::unique_ptr<Door>>& doors)
{
  for (std::unique_ptr<Door>& door : doors)
  {
    m_doors.push_back(std::move(door));
  }

  return true;
}

void Dungeon::add_adjacency(int roomID, Room::adjacent_room adj)
{
  auto search = m_adjacency_map.find(roomID);
  
  if (search != m_adjacency_map.end())
  {
    search->second.push_back(adj);
  }
  else
  {
    m_adjacency_map.emplace(roomID, std::vector<Room::adjacent_room>());
    m_adjacency_map.at(roomID).push_back(adj);
  }
}

string Dungeon::get_hero_timer()
{
	return get_timer_string(m_hero_timer);
}

string Dungeon::get_boss_fight_timer()
{
	return get_timer_string(m_boss_fight_timer);
}

bool Dungeon::should_spawn_hero()
{
	return m_should_spawn_hero;
}

bool Dungeon::hero_has_spawned()
{
	return m_hero_has_spawned;
}

void Dungeon::spawn_hero()
{
	m_hero_has_spawned = true;
}

void Dungeon::set_boss(Boss* boss)
{
	m_boss = boss;
}

Boss* Dungeon::get_boss()
{
	return m_boss;
}

void Dungeon::start_boss_fight()
{
	m_boss_fight_has_started = true;
}

bool Dungeon::has_boss_fight_started()
{
	return m_boss_fight_has_started;
}

bool Dungeon::has_boss_fight_ended()
{
	return m_boss_fight_has_ended;
}