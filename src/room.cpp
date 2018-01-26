// room.cpp

#include "room.hpp"

#define NUM_WALLS 52
#define NUM_FLOORS 140
#define SPRITE_SIZE 64.f

vec2 wall_positions[] = {
    {0.f, 0.f},   {1.f, 0.f},   {2.f, 0.f},   {3.f, 0.f},   {4.f, 0.f},
    {5.f, 0.f},   {6.f, 0.f},   {7.f, 0.f},   {8.f, 0.f},   {9.f, 0.f},
    {10.f, 0.f},  {11.f, 0.f},  {12.f, 0.f},  {13.f, 0.f},  {14.f, 0.f},
    {15.f, 0.f},  {0.f, 1.f},   {15.f, 1.f},  {0.f, 2.f},   {15.f, 2.f},
    {0.f, 3.f},   {15.f, 3.f},  {0.f, 4.f},   {15.f, 4.f},  {0.f, 5.f},
    {15.f, 5.f},  {0.f, 6.f},   {15.f, 6.f},  {0.f, 7.f},   {15.f, 7.f},
    {0.f, 8.f},   {15.f, 8.f},  {0.f, 9.f},   {15.f, 9.f},  {0.f, 10.f},
    {15.f, 10.f}, {0.f, 11.f},  {1.f, 11.f},  {2.f, 11.f},  {3.f, 11.f},
    {4.f, 11.f},  {5.f, 11.f},  {6.f, 11.f},  {7.f, 11.f},   {8.f, 11.f},
    {9.f, 11.f},  {10.f, 11.f}, {11.f, 11.f}, {12.f, 11.f}, {13.f, 11.f},
    {14.f, 11.f}, {15.f, 11.f},
};

vec2 floor_positions[] = {
	{1.f, 1.f},  {2.f, 1.f},  {3.f, 1.f}, {4.f, 1.f},  {5.f, 1.f},  {6.f, 1.f},
	{7.f, 1.f},  {8.f, 1.f},  {9.f, 1.f}, {10.f, 1.f}, {11.f, 1.f}, {12.f, 1.f},
	{13.f, 1.f}, {14.f, 1.f}, //
	{1.f, 2.f},  {2.f, 2.f},  {3.f, 2.f}, {4.f, 2.f},  {5.f, 2.f},  {6.f, 2.f},
	{7.f, 2.f},  {8.f, 2.f},  {9.f, 2.f}, {10.f, 2.f}, {11.f, 2.f}, {12.f, 2.f},
	{13.f, 2.f}, {14.f, 2.f}, //
	{1.f, 3.f},  {2.f, 3.f},  {3.f, 3.f}, {4.f, 3.f},  {5.f, 3.f},  {6.f, 3.f},
	{7.f, 3.f},  {8.f, 3.f},  {9.f, 3.f}, {10.f, 3.f}, {11.f, 3.f}, {12.f, 3.f},
	{13.f, 3.f}, {14.f, 3.f}, //
	{1.f, 4.f},  {2.f, 4.f},  {3.f, 4.f}, {4.f, 4.f},  {5.f, 4.f},  {6.f, 4.f},
	{7.f, 4.f},  {8.f, 4.f},  {9.f, 4.f}, {10.f, 4.f}, {11.f, 4.f}, {12.f, 4.f},
	{13.f, 4.f}, {14.f, 4.f}, //
	{1.f, 5.f},  {2.f, 5.f},  {3.f, 5.f}, {4.f, 5.f},  {5.f, 5.f},  {6.f, 5.f},
	{7.f, 5.f},  {8.f, 5.f},  {9.f, 5.f}, {10.f, 5.f}, {11.f, 5.f}, {12.f, 5.f},
	{13.f, 5.f}, {14.f, 5.f}, //
	{1.f, 6.f},  {2.f, 6.f},  {3.f, 6.f}, {4.f, 6.f},  {5.f, 6.f},  {6.f, 6.f},
	{7.f, 6.f},  {8.f, 6.f},  {9.f, 6.f}, {10.f, 6.f}, {11.f, 6.f}, {12.f, 6.f},
	{13.f, 6.f}, {14.f, 6.f}, //
	{1.f, 7.f},  {2.f, 7.f},  {3.f, 7.f}, {4.f, 7.f},  {5.f, 7.f},  {6.f, 7.f},
	{7.f, 7.f},  {8.f, 7.f},  {9.f, 7.f}, {10.f, 7.f}, {11.f, 7.f}, {12.f, 7.f},
	{13.f, 7.f}, {14.f, 7.f}, //
	{1.f, 8.f},  {2.f, 8.f},  {3.f, 8.f}, {4.f, 8.f},  {5.f, 8.f},  {6.f, 8.f},
	{7.f, 8.f},  {8.f, 8.f},  {9.f, 8.f}, {10.f, 8.f}, {11.f, 8.f}, {12.f, 8.f},
	{13.f, 8.f}, {14.f, 8.f}, //
	{1.f, 9.f},  {2.f, 9.f},  {3.f, 9.f}, {4.f, 9.f},  {5.f, 9.f},  {6.f, 9.f},
	{7.f, 9.f},  {8.f, 9.f},  {9.f, 9.f}, {10.f, 9.f}, {11.f, 9.f}, {12.f, 9.f},
	{13.f, 9.f}, {14.f, 9.f}, //
	{1.f, 10.f},  {2.f, 10.f},  {3.f, 10.f}, {4.f, 10.f},  {5.f, 10.f},  {6.f, 10.f},
	{7.f, 10.f},  {8.f, 10.f},  {9.f, 10.f}, {10.f, 10.f}, {11.f, 10.f}, {12.f, 10.f},
	{13.f, 10.f}, {14.f, 10.f}, //
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
		if (!m_walls[i].init(wall_positions[i]*SPRITE_SIZE))    // each position needed to be mult by 64 as each spite is 64x64 pixels
															// and 1 pixel = 1 in world coords
		{
			return false;
		}
	}

	for (size_t i = 0; i < NUM_FLOORS; ++i)
	{
		m_floors.push_back({});
		if (!m_floors[i].init(floor_positions[i] * SPRITE_SIZE))
		{
			return false;
		}
	}

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