// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"

Dungeon::Dungeon() : 
	GameObject()
{
}

Dungeon::~Dungeon() {}

bool Dungeon::init() 
{
  RoomParser parser;

  /**************1st Room ****************/
  m_rooms.emplace_back();
	Room& new_room = m_rooms.back();
	if (!new_room.init()) 
	{
		fprintf(stderr, "Failed to init room.\n");
		return false;
	}

  new_room.set_pos(
      {128.f, 52.f}); // temporary values, eventually we will want to have
                      // a parser that creates the dungeon layouts

  if (!parser.parseRoom(new_room, room_path("1.rm")))
  {
    return false;
  }

  /**************2nd Room ****************/
  m_rooms.emplace_back();
  Room& new_room_2 = m_rooms.back();
  if (!new_room_2.init())
  {
    fprintf(stderr, "Failed to init room.\n");
    return false;
  }

  new_room_2.set_pos({ 128.f, -800.f }); // temporary values, eventually we will want to have
                                     // a parser that creates the dungeon layouts
  if (!parser.parseRoom(new_room_2, room_path("1.rm")))
  {
    return false;
  }

	return true;
}

void Dungeon::destroy()
{
	for (Room& room : m_rooms)
	{
		room.destroy();
	}
}

void Dungeon::clean(vec2 janitor_pos)
{
	for (Room& room : m_rooms)
	{
		std::vector<Puddle>& cleanables = room.get_cleanables();
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

void Dungeon::update_current(float ms)
{
}

void Dungeon::update_children(float ms)
{
	for (Room& room : m_rooms)
	{
		room.update(ms);
	}
}

void Dungeon::draw_current(const mat3& projection, const mat3& current_transform)
{
}

void Dungeon::draw_children(const mat3& projection, const mat3& current_transform)
{
	for (Room& room : m_rooms)
	{
		room.draw(projection, current_transform);
	}
}