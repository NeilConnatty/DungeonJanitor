// toweldispenser.cpp

#include "toweldispenser.hpp"

Texture TowelDispenser::towel_texture;

bool TowelDispenser::init(vec2 pos)
{
  return FloorObject::init(pos);
}


Texture& TowelDispenser::get_texture()
{
  return towel_texture;
}


bool TowelDispenser::load_texture()
{
  if (!towel_texture.load_from_file(textures_path("dungeon1/d1_wallobject_paperdispenser.png")))
  {
    fprintf(stderr, "Failed to load dispenser texture\n");
    return false;
  }
  return true;
}
