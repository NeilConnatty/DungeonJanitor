#pragma once

#include "common.hpp"
#include "floorobject.hpp"

#define NUM_DESK_TEXTURES 3

class Desk : public FloorObject
{
private:
  static Texture large_desk_textures[NUM_DESK_TEXTURES];
  static Texture small_desk_textures[NUM_DESK_TEXTURES];
  bool m_small_desk;
  size_t texture_index;

public:
  bool init() override { return init({ 0.f,0.f }, false); }
  bool init(vec2 pos) override { return init(pos, false); }
  bool init(vec2 pos, bool small_desk);
  Texture& get_texture() override;
  bool load_texture() override;
};
