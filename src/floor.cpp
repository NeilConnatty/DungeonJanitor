// floor.cpp

#include "floor.hpp"

Floor::Floor() {}

Floor::~Floor() {}

bool Floor::init(vec2 position)
{
	if (!floor_texture.is_valid())
	{
		if (!floor_texture.load_from_file(textures_path("floor.png")))
		{
			fprintf(stderr, "Failed to load floor texture");
			return false;
		}
	}
	
	m_position = position;

	return true;
}

void Floor::destroy()
{
}

void Floor::set_position(vec2 position)
{
	m_position = position;
}

void Floor::draw(const mat3& projection)
{

}
