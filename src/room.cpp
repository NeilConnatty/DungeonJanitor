// room.cpp

#include "room.hpp"

#define NUM_WALLS 24

vec2 wall_positions[] = {
    {0.f, 0.f}, {1.f, 0.f}, {2.f, 0.f}, {3.f, 0.f}, {4.f, 0.f}, {5.f, 0.f},
    {6.f, 0.f}, {7.f, 0.f}, {0.f, 1.f}, {7.f, 1.f}, {0.f, 2.f}, {7.f, 2.f},
    {0.f, 3.f}, {7.f, 3.f}, {0.f, 4.f}, {7.f, 4.f}, {0.f, 5.f}, {1.f, 5.f},
    {2.f, 5.f}, {3.f, 5.f}, {4.f, 5.f}, {5.f, 5.f}, {6.f, 5.f}, {7.f, 5.f},
};

vec2 floor_positions[] = {
    {1.f, 1.f}, {2.f, 1.f}, {3.f, 1.f}, {4.f, 1.f}, {5.f, 1.f}, {6.f, 1.f},
    {1.f, 2.f}, {2.f, 2.f}, {3.f, 2.f}, {4.f, 2.f}, {5.f, 2.f}, {6.f, 2.f},
    {1.f, 3.f}, {2.f, 3.f}, {3.f, 3.f}, {4.f, 3.f}, {5.f, 3.f}, {6.f, 3.f},
    {1.f, 4.f}, {2.f, 4.f}, {3.f, 4.f}, {4.f, 4.f}, {5.f, 4.f}, {6.f, 4.f},
};

Room::Room()
{
}

Room::~Room()
{
}

// Creates a room
bool Room::init(vec2 position)
{
	for (size_t i = 0; i < NUM_WALLS; ++i)
	{
		m_walls.push_back({});
		if (!m_walls[i].init(wall_positions[i]*16.f))    // each position needed to be mult by 16 as each spite is 16x16 pixels
														 // and 1 pixel = 1 in world coords (for some reason, not sure how we change
														 // as this is janky AF)
		{
			return false;
		}
	}

	for (size_t i = 0; i < NUM_WALLS; ++i)
	{
		m_floors.push_back({});
		if (!m_floors[i].init(floor_positions[i] * 16.f))
		{
			return false;
		}
	}

	m_position = position;
	m_scale = { 4.f, 4.f };

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

// Steps the game ahead by ms milliseconds
bool Room::update(float ms)
{
	return true;
}

// Renders our room
void Room::draw(const mat3& projection, const mat3& parent_transform)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

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

void Room::set_position(vec2 pos)
{
	m_position = pos;
}