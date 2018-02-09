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
	m_rooms.emplace_back();
	Room& new_room = m_rooms.back();
	if (!new_room.init()) 
	{
		fprintf(stderr, "Failed to init room.\n");
		return false;
	}

	new_room.set_pos(
		{128.f, 52.f});	  // temporary values, as we don't have a real camera yet,
			              // so positions are in pixels. we will eventually have a
				          // dungeon object that contains multiple rooms.

	RoomParser parser;
	parser.parseRoom(new_room, "../data/rooms/1.rm");

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