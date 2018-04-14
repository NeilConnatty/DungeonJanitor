#pragma once

#include "common.hpp"
#include "floorobject.hpp"

#define NUM_STALL_TEXTURES 3

class BathroomStall : public FloorObject
{
private:
  static Texture stall_textures[NUM_STALL_TEXTURES];
  size_t m_texture_index;

public:
  bool init() override { return init({ 0.f,0.f }); }
  bool init(vec2 pos) override;
  Texture& get_texture() override;
  bool load_texture() override;
};
