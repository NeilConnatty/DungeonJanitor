// graffiti.cpp
// Must go on walls that face the camera
#include "graffiti.hpp"

Texture Graffiti::graffiti_texture;

Graffiti::Graffiti() {}

Graffiti::~Graffiti() {}

bool Graffiti::init(vec2 pos)
{
	pos.y = pos.y + 30;
	return Cleanable::init(pos);
}

Texture& Graffiti::get_texture()
{
	return graffiti_texture;
}

bool Graffiti::load_texture()
{
	if (!get_texture().is_valid())
	{
		srand(time(0));
		int random = rand() % 3 + 1;
		if (random == 1)
		{
			if (!graffiti_texture.load_from_file(dungeon1_textures_path("d1_cleanable_graffiti-1.png")))
			{
				fprintf(stderr, "Failed to load graffiti1 texture\n");
				return false;
			}
		}
		else if (random == 2)
		{
			if (!graffiti_texture.load_from_file(dungeon1_textures_path("d1_cleanable_graffiti-2.png")))
			{
				fprintf(stderr, "Failed to load graffiti2 texture\n");
				return false;
			}
		}
		else
		{
			if (!graffiti_texture.load_from_file(dungeon1_textures_path("d1_cleanable_graffiti-3.png")))
			{
				fprintf(stderr, "Failed to load graffiti3 texture\n");
				return false;
			}
		}
	}
	m_size = { static_cast<float>(graffiti_texture.width), static_cast<float>(graffiti_texture.height) };
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	return true;
}