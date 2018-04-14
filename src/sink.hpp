#pragma once

#include "floorobject.hpp"

class Sink : public FloorObject
{
private:
  static Texture sink_texture;

public:
  bool init() override { return init({ 0.f,0.f }); }
  bool init(vec2 pos) override;
  Texture& get_texture() override;
  bool load_texture() override;
};