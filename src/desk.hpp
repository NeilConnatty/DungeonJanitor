#pragma once

#include "common.hpp"
#include "floorobject.hpp"

class Desk : public FloorObject
{
private:
  static Texture desk_texture;

public:
  bool init(vec2 pos);
  Texture& get_texture() override;
  bool load_texture() override;
};
