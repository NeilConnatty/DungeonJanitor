#pragma once

#include "floorobject.hpp"

#define NUM_CABINET_TEXTURES 2

class FileCabinet : public FloorObject
{
private:
  static Texture cabinet_textures[NUM_CABINET_TEXTURES];
  size_t m_texture_index;

public:
  bool init() override { return init({ 0.f,0.f }); }
  bool init(vec2 pos) override;
  Texture& get_texture() override;
  bool load_texture() override;
};