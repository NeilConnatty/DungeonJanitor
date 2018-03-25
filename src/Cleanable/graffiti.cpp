// graffiti.cpp
// Must go on walls that face the camera
#include "graffiti.hpp"

Texture Graffiti::graffiti_textures[NUM_GRAFFITI_TEXTURES];

Graffiti::Graffiti() {}

Graffiti::~Graffiti() {}

bool Graffiti::init(vec2 pos)
{
	pos.y = pos.y + 32;
	default_random_engine rng;
	uniform_int_distribution<int> dist;
	rng = default_random_engine(random_device()());
	dist = uniform_int_distribution<int>(0, NUM_GRAFFITI_TEXTURES - 1);
	m_texture_index = dist(rng);
	return Cleanable::init(pos);
}

Texture& Graffiti::get_texture()
{
	return graffiti_textures[m_texture_index];
}

bool Graffiti::load_texture()
{
	if (!graffiti_textures[0].is_valid())
	{
		if (!graffiti_textures[0].load_from_file(textures_path("dungeon1/d1_cleanable_graffiti-1.png")))
		{
			fprintf(stderr, "Failed to load graffiti1 texture\n");
			return false;
		}
		if (!graffiti_textures[1].load_from_file(textures_path("dungeon1/d1_cleanable_graffiti-2.png")))
		{
			fprintf(stderr, "Failed to load graffiti2 texture\n");
			return false;
		}
		if (!graffiti_textures[2].load_from_file(textures_path("dungeon1/d1_cleanable_graffiti-3.png")))
		{
			fprintf(stderr, "Failed to load graffiti3 texture\n");
			return false;
		}
	}
	m_size = { static_cast<float>(graffiti_textures[0].width), static_cast<float>(graffiti_textures[0].height) };
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	return true;
}

bool Graffiti::clean()
{
	m_enabled = false;
	return true;
}
