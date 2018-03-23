// garbage.cpp

#include "garbage.hpp"

Texture Garbage::garbage_texture_empty;
Texture Garbage::garbage_texture_full;

Garbage::Garbage() {}

Garbage::~Garbage() {}

bool Garbage::init(vec2 pos)
{
	m_is_empty = false;
	return Cleanable::init(pos);
}

Texture& Garbage::get_texture()
{
	if (m_is_empty)
	{
		return garbage_texture_empty;
	}
	else {
		return garbage_texture_full;
	}
}

bool Garbage::load_texture()
{
	if (!garbage_texture_full.is_valid())
	{
		if (!garbage_texture_full.load_from_file(dungeon1_textures_path("d1_floorobject_garbagecan-blue-full-1.png")))
		{
			fprintf(stderr, "Failed to load full blue garbagecan texture\n");
			return false;
		}
		if (!garbage_texture_empty.load_from_file(dungeon1_textures_path("d1_floorobject_garbagecan-blue-empty-1.png")))
		{
			fprintf(stderr, "Failed to load empty blue garbagecan texture\n");
			return false;
		}
	}
	return true;
}

void Garbage::clean()
{
	m_is_empty = true;
}