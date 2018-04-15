// mirror.cpp

#include "mirror.hpp"

Texture Mirror::mirror_texture;

bool Mirror::init(vec2 pos)
{
  return FloorObject::init(pos);
}


Texture& Mirror::get_texture()
{
  return mirror_texture;
}


bool Mirror::load_texture()
{
  if (!mirror_texture.load_from_file(textures_path("dungeon1/d1_wallobject_mirror.png")))
  {
    fprintf(stderr, "Failed to load cactus texture\n");
    return false;
  }
  return true;
}
