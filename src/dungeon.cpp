// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"
#include "ValueIteration.hpp" // testing
#include <iostream>

#define ARTIFACT_VALUE 5

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

	room->setDungeonTransform(identity_matrix);
  }

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
	m_healthBar = NULL;
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
			room_ptr->increment_activated_artifacts();
		}
	}
}

void Dungeon::update_current(float ms)
{
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
