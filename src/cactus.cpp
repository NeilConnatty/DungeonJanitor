// cactus.cpp

#include "cactus.hpp"

Texture Cactus::cactus_texture;

bool Cactus::init(vec2 pos)
{
  return FloorObject::init(pos);
}


Texture& Cactus::get_texture()
{
  return cactus_texture;
}


bool Cactus::load_texture()
{
  if (!cactus_texture.load_from_file(textures_path("dungeon1/d1_floorobject_cactus-l-1.png")))
  {
    fprintf(stderr, "Failed to load cactus texture\n");
    return false;
  }
  return true;
}
