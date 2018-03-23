// puddle.cpp

#include "puddle.hpp"

Texture Puddle::puddle_texture;

Puddle::Puddle() {}

Puddle::~Puddle() {}

Texture& Puddle::get_texture()
{
	return puddle_texture;
}

bool Puddle::load_texture()
{
	if (!get_texture().is_valid())
	{
		if (!puddle_texture.load_from_file(dungeon1_textures_path("d1_cleanable_puddle-l.png")))
		{
			fprintf(stderr, "Failed to load puddle texture\n");
			return false;
		}
	}
	return true;
}

bool Puddle::clean()
{
	m_enabled = false;
	return true;
}