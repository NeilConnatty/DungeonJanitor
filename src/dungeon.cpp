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
  if (!parser.parseDungeon(m_rooms, dungeon_path("1.dn")))
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
      m_emitters.emplace_back();
      m_emitters.back().init(janitor_room_position, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0.0f, 1.0f, 10.0f, 20);
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

  return true;
}

void Dungeon::destroy()
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->destroy();
	}
	for (Emitter emitter : m_emitters)
	{
		emitter.destroy();
	}
}

vector<unique_ptr<Room>>& Dungeon::get_rooms()
{
	return m_rooms;
}

void Dungeon::clean()
{
	// for (std::unique_ptr<Room>& room : m_rooms)
	// {
	// 	std::vector<Puddle>& cleanables = room->get_cleanables();
	// 	for (Puddle& p : cleanables)
	// 	{
	// 		if (p.is_enabled())
	// 		{
	// 			// Collision stuff goes here
	// 			if (true)
	// 			{
	// 				p.toggle_enable();
	// 			}
	// 		}
	// 	}
	// }
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
}

void Dungeon::update_children(float ms)
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->update(ms);
	}
	for (Emitter emitter : m_emitters)
	{
		emitter.update(ms);
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
	for (Emitter emitter : m_emitters)
	{
		emitter.draw(projection, current_transform);
	}
}
