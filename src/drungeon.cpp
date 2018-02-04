// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"

Dungeon::Dungeon() : 
	m_position({0.f, 0.f})
{}

Dungeon::~Dungeon() {}

bool Dungeon::init() 
{
	m_rooms.push_back(std::make_unique<Room>());
	Room::ptr &new_room = m_rooms.back();
	if (!new_room->init()) 
	{
		fprintf(stderr, "Failed to init room.\n");
		return false;
	}

	new_room->set_position(
		{128.f, 52.f});	  // temporary values, as we don't have a real camera yet,
			              // so positions are in pixels. we will eventually have a
				          // dungeon object that contains multiple rooms.

	RoomParser parser;
	parser.parseRoom(*new_room, "../data/rooms/1.rm");

	return true;
}

void Dungeon::destroy()
{
	for (Room::ptr& room : m_rooms)
	{
		room->destroy();
	}
}

void Dungeon::set_position(vec2 pos)
{
	m_position = pos;
}

void Dungeon::update_current(float ms)
{
}

void Dungeon::update_children(float ms)
{
	for (Room::ptr& room : m_rooms)
	{
		room->update(ms);
	}
}

void Dungeon::update(float ms)
{
	update_current(ms);
	update_children(ms);
}

void Dungeon::draw(const mat3& projection, const mat3& parent_transform)
{
	transform_begin();
	transform_translate(m_position);
	transform_end();

	mat3 final_transform = parent_transform * transform;

	for (Room::ptr& room : m_rooms)
	{
		room->draw(projection, final_transform);
	}
}