// puddle.cpp

#include "puddle.hpp"

Texture Puddle::puddle_texture;

Puddle::Puddle() {}

Puddle::~Puddle() {}

bool Puddle::init(vec2 pos)
{
	m_sound = Mix_LoadWAV(audio_path("puddle.wav"));
	return Cleanable::init(pos);
}

Texture& Puddle::get_texture()
{
	return puddle_texture;
}

bool Puddle::load_texture()
{
	if (!get_texture().is_valid())
	{
		if (!puddle_texture.load_from_file(textures_path("dungeon1/d1_cleanable_puddle-l.png")))
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