// filecabinet.cpp

#include "filecabinet.hpp"
#include <random>

Texture FileCabinet::cabinet_textures[NUM_CABINET_TEXTURES];

bool FileCabinet::init(vec2 pos)
{
  std::default_random_engine rng;
  std::uniform_int_distribution<int> dist;
  rng = std::default_random_engine(std::random_device()());
  dist = std::uniform_int_distribution<int>(0, NUM_CABINET_TEXTURES - 1);
  m_texture_index = dist(rng);
  return FloorObject::init(pos);
}

Texture& FileCabinet::get_texture()
{
  return cabinet_textures[m_texture_index];
}

bool FileCabinet::load_texture()
{
  if (!cabinet_textures[0].load_from_file(textures_path("dungeon1/d1_floorobject_filecabinet-1.png")))
  {
    fprintf(stderr, "Failed to load file cabinet texture\n");
    return false;
  }

  if (!cabinet_textures[1].load_from_file(textures_path("dungeon1/d1_floorobject_filecabinet-2.png")))
  {
    fprintf(stderr, "Failed to load file cabinet texture\n");
    return false;
  }

  return true;
}
