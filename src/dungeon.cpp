// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"
#include "ValueIteration.hpp" // testing
#include <iostream>

#define ARTIFACT_VALUE 5
#define HERO_TIME_TO_SPAWN 60000.f

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
  m_emitters.emplace_back();
  // vec2 velocity = {10.0f, 1.0f};
  // vec4 color = {1.0f, 0.0f, 0.0f, 1.0f};
  m_emitters.back().init({10.0f, 10.0f}, {40, 40}, {10.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 10.0f, 20, {0.5f, 1.0f});
  m_hero_timer = HERO_TIME_TO_SPAWN; // Three minutes in milliseconds
  m_should_spawn_hero = false;
  m_hero_has_spawned = false;
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

  m_healthBar->set_percent_filled(get_percent_dungeon_cleaned());
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
	double minutesRemainder = (m_hero_timer) / 60000;
	int minutes = minutesRemainder;
	double secondsRemainder = (minutesRemainder - minutes) * 60;
	int seconds = secondsRemainder;
	string minutes_str = to_string(minutes);
	string seconds_str = to_string(seconds);

	if (minutes <= 0 && seconds < 0)
	{
		return "0:00";
	}
	if (seconds < 10)
	{
		seconds_str = "0" + seconds_str;
	}
	return  minutes_str + ":" + seconds_str;
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
