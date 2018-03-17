// graffiti.cpp
// Must go on walls that face the camera
#include "graffiti.hpp"

Texture Graffiti::graffiti_texture;

Graffiti::Graffiti() {}

Graffiti::~Graffiti() {}

Texture& Graffiti::get_texture()
{
	return graffiti_texture;
}

bool Graffiti::load_texture()
{
	if (!get_texture().is_valid())
	{
		if (!graffiti_texture.load_from_file(dungeon1_textures_path("d1_cleanable_graffiti-1.png")))
		{
			fprintf(stderr, "Failed to load puddle texture\n");
			return false;
		}
	}

	m_scale.x = 1.f;
	m_scale.y = 1.f;
	return true;
}