// desk.cpp

#include "desk.hpp"
#include <random>

Texture Desk::small_desk_textures[NUM_DESK_TEXTURES];
Texture Desk::large_desk_textures[NUM_DESK_TEXTURES];

bool Desk::init(vec2 pos, bool small_desk)
{
  m_small_desk = small_desk;
  std::default_random_engine rng;
  std::uniform_int_distribution<int> dist;
  rng = std::default_random_engine(std::random_device()());
  dist = std::uniform_int_distribution<int>(0, NUM_DESK_TEXTURES - 1);
  texture_index = dist(rng);
  return FloorObject::init(pos);
}

Texture& Desk::get_texture()
{
  if (m_small_desk)
  {
    return small_desk_textures[texture_index];
  }
  // else
  return large_desk_textures[texture_index];
}

bool Desk::load_texture()
{
  if (!large_desk_textures[0].load_from_file(textures_path("dungeon1/d1_floorobject_desk-l-1.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }

  if (!large_desk_textures[1].load_from_file(textures_path("dungeon1/d1_floorobject_desk-l-2.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }

  if (!large_desk_textures[2].load_from_file(textures_path("dungeon1/d1_floorobject_desk-l-3.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }

  if (!small_desk_textures[0].load_from_file(textures_path("dungeon1/d1_floorobject_desk-s-1.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }

  if (!small_desk_textures[1].load_from_file(textures_path("dungeon1/d1_floorobject_desk-s-2.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }

  if (!small_desk_textures[2].load_from_file(textures_path("dungeon1/d1_floorobject_desk-s-3.png")))
  {
    fprintf(stderr, "Failed to load desk texture\n");
    return false;
  }

  return true;
}
