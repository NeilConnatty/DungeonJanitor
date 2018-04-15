// sink.cpp

#include "sink.hpp"

Texture Sink::sink_texture;

bool Sink::init(vec2 pos)
{
  return FloorObject::init(pos);
}


Texture& Sink::get_texture()
{
  return sink_texture;
}


bool Sink::load_texture()
{
  if (!sink_texture.load_from_file(textures_path("dungeon1/d1_wallobject_sink.png")))
  {
    fprintf(stderr, "Failed to load cactus texture\n");
    return false;
  }
  return true;
}
