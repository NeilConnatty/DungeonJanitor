// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"
#include "ValueIteration.hpp" // testing
#include <iostream>

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
  m_hero_timer = 180000.f;
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
}

vector<unique_ptr<Room>>& Dungeon::get_rooms()
{
	return m_rooms;
}

void Dungeon::clean()
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		std::vector<Puddle>& cleanables = room->get_cleanables();
		for (Puddle& p : cleanables)
		{
			if (p.is_enabled())
			{
				// Collision stuff goes here
				if (true)
				{
					p.toggle_enable();
				}
			}
		}
	}
}

void Dungeon::activate_artifact()
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		Room* room_ptr = room.get();
		if (room_ptr->containsUndiscoveredArtifact())
		{
			room_ptr->get_artifact()->set_active(true);
		}
	}
}

void Dungeon::update_current(float ms)
{
	m_hero_timer -= ms;

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
