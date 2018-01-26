// wall.cpp

#include "wall.hpp"

Texture Wall::wall_texture;

Wall::Wall() {}
Wall::~Wall() {}

bool Wall::init()
{
	return true;
}

void Wall::destroy() {}

void Wall::set_position(vec2 position) {}

vec2 Wall::get_bounding_box() const 
{
	return {0.f, 0.f};
}

void Wall::draw(const mat3& projection, const mat3& parent_transform)
{

}