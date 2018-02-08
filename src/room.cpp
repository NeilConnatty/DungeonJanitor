// room.cpp

#include "room.hpp"
#include <iostream>
using namespace std;

#define SPRITE_SIZE 64.f

Room::Room()
{
}

Room::~Room()
{
}

// Creates a room
bool Room::init(vec2 position)
{
	m_position = position;
	m_scale = { 1.f, 1.f };

	return true;
}

bool Room::init()
{
	return init({ 0.f, 0.f });
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

void Room::update_current(float ms)
{
}

void Room::update_children(float ms)
// at the moment the room's only children are walls and floors,
// which do not need to be updated

// puddles will need to be updated
{}

void Room::draw_current(const mat3& projection, const mat3& current_transform)
{
}

// Renders our room
void Room::draw_children(const mat3& projection, const mat3& current_transform)
{

	for (Floor f : m_floors)
	{
		f.draw(projection, current_transform);
	}

	for (Wall w : m_walls)
	{
		w.draw(projection, current_transform);
	}
	
	for (Puddle p : m_puddles)
	{
		p.draw(projection, current_transform);
	}
}


bool Room::add_floors(vector<vec2>& positions)
{
	for (vec2& pos : positions)
	{
		m_floors.push_back({});
		if (!m_floors.back().init(pos * SPRITE_SIZE))	 // each position needed to be mult by 64 as each spite is 64x64 pixels
														 // and 1 pixel = 1 in world coords
		{
			return false;
		}
	}

	return true;
}

bool Room::add_walls(vector<vec2>& positions)
{
	for (vec2& pos : positions)
	{
		m_walls.push_back({});
		if (!m_walls.back().init(pos * SPRITE_SIZE))    // each position needed to be mult by 64 as each spite is 64x64 pixels
														// and 1 pixel = 1 in world coords
		{
			return false;
		}
	}

	return true;
}

bool Room::add_cleanables(vector<vec2>& puddle_positions)
{
	for (vec2& pos : puddle_positions)
	{
		m_puddles.push_back({});
		if (!m_puddles.back().init(pos * SPRITE_SIZE))

		{
			return false;
		}
	}
	return true;
}

bool Room::handle_collision(Janitor player)
{
	bool collision_detected = false;

	for (auto& wall : m_walls)
	{
		if (player.collides_with(wall))
		{
			// Handle collision here
			collision_detected = true;
			cout << "Collision Detected";
		}
	}

	return collision_detected;
}
