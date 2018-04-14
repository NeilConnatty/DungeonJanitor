// bathroomstall.cpp

#include "bathroomstall.hpp"
#include <random>

Texture BathroomStall::stall_textures[NUM_STALL_TEXTURES];

bool BathroomStall::init(vec2 pos)
{
  std::default_random_engine rng;
  std::uniform_int_distribution<int> dist;
  rng = std::default_random_engine(std::random_device()());
  dist = std::uniform_int_distribution<int>(0, NUM_STALL_TEXTURES - 1);
  m_texture_index = dist(rng);
  return FloorObject::init(pos);
}

Texture& BathroomStall::get_texture()
{
  return stall_textures[m_texture_index];
}

bool BathroomStall::load_texture()
{
  if (!stall_textures[0].load_from_file(textures_path("dungeon1/d1_floorobject_bathroomstall-1.png")))
  {
    fprintf(stderr, "Failed to load bathroom stall texture\n");
    return false;
  }

  if (!stall_textures[1].load_from_file(textures_path("dungeon1/d1_floorobject_bathroomstall-2.png")))
  {
    fprintf(stderr, "Failed to load bathroom stall texture\n");
    return false;
  }

  if (!stall_textures[2].load_from_file(textures_path("dungeon1/d1_floorobject_bathroomstall-3.png")))
  {
    fprintf(stderr, "Failed to load bathroom stall texture\n");
    return false;
  }

  return true;
}
