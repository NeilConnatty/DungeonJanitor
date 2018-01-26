// room.cpp

#include "room.hpp"

Room::Room()
{
}

Room::~Room()
{
}

// Creates a room
bool Room::init()
{
	m_floors.push_back({});

	if (!m_floors[0].init({ 1.f, 1.f }))
	{
		return false;
	}

	return true;
}

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
}

// Steps the game ahead by ms milliseconds
bool Room::update(float ms)
{
	return true;
}

// Renders our room
void Room::draw(const mat3& projection, const mat3& parent_transform)
{
	mat3 final_transform = parent_transform * transform;

	for (Floor f : m_floors)
	{
		f.draw(projection, final_transform);
	}

	for (Wall w : m_walls)
	{
		w.draw(projection, final_transform);
	}
}