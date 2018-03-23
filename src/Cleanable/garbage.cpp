// garbage.cpp

#include "garbage.hpp"

Texture Garbage::garbage_texture_blue[NUM_GARBAGE_TEXTURES];
Texture Garbage::garbage_texture_green[NUM_GARBAGE_TEXTURES];

Garbage::Garbage() {}

Garbage::~Garbage() {}

bool Garbage::init(vec2 pos)
{
	m_is_empty = false;
	default_random_engine rng;
	uniform_int_distribution<int> dist;
	rng = default_random_engine(random_device()());
	dist = uniform_int_distribution<int>(0, NUM_GARBAGE_TEXTURES - 1);
	m_texture_index = dist(rng);
	return Cleanable::init(pos);
}

Texture& Garbage::get_texture()
{
	if (m_texture_index == 0)
	{
		if (m_is_empty)
		{
			return garbage_texture_blue[1];
		}
		else {
			return garbage_texture_blue[0];
		}
	}
	else
	{
		if (m_is_empty)
		{
			return garbage_texture_green[1];
		}
		else {
			return garbage_texture_green[0];
		}
	}
}

bool Garbage::load_texture()
{
	if (!garbage_texture_blue[0].is_valid())
	{
		if (!garbage_texture_blue[0].load_from_file(dungeon1_textures_path("d1_floorobject_garbagecan-blue-full-1.png")))
		{
			fprintf(stderr, "Failed to load full blue garbagecan texture\n");
			return false;
		}
		if (!garbage_texture_blue[1].load_from_file(dungeon1_textures_path("d1_floorobject_garbagecan-blue-empty-1.png")))
		{
			fprintf(stderr, "Failed to load empty blue garbagecan texture\n");
			return false;
		}
		if (!garbage_texture_green[0].load_from_file(dungeon1_textures_path("d1_floorobject_garbagecan-green-full-1.png")))
		{
			fprintf(stderr, "Failed to load full green garbagecan texture\n");
			return false;
		}
		if (!garbage_texture_green[1].load_from_file(dungeon1_textures_path("d1_floorobject_garbagecan-green-empty-1.png")))
		{
			fprintf(stderr, "Failed to load empty green garbagecan texture\n");
			return false;
		}
	}
	return true;
}

bool Garbage::clean()
{
	if (!m_is_empty)
	{
		m_is_empty = true;
		return true;
	}
	else
	{
		return false;
	}
}