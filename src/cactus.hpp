#pragma once

#include "floorobject.hpp"

class Cactus : public FloorObject
{
private:
  static Texture cactus_texture;

public:
  bool init() override { return init({ 0.f,0.f }); }
  bool init(vec2 pos) override;
  Texture& get_texture() override;
  bool load_texture() override;
};