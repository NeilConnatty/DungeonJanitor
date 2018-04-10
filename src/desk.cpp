// desk.cpp

#include "desk.hpp"

Texture Desk::desk_texture;

bool Desk::init(vec2 pos)
{
  return FloorObject::init(pos);
}

Texture& Desk::get_texture()
{
  return desk_texture;
}

bool Desk::load_texture()
{
  if (!desk_texture.load_from_file(textures_path("dungeon1/d1_floorobject_desk-l-1.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }
}
