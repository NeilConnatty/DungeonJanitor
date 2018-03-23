#pragma once

#include "common.hpp"
#include "door.hpp"
#include "gameobject.hpp"
#include "floor.hpp"

#include <array>

class Door : public GameObject
{
public:
  static Texture door_textures[1];

  enum door_dir
  {
    VERTICAL = 0,
    HORIZONTAL = 1
  };

  using door_pair = std::pair<vec2, door_dir>;

public:
  Door();
  ~Door();

  bool init() override { return init({ 0.f,0.f }, VERTICAL); }
  bool init(door_pair pair) { return init(pair.first, pair.second); }
  bool init(vec2 pos, door_dir dir);
  void destroy() override;
  door_dir get_dir();

protected:
  void update_current(float ms) override;
  void update_children(float ms) override;
  void draw_current(const mat3& projection, const mat3& current_transform) override;
  void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
  std::array<Floor, 3> m_floors;
  door_dir m_door_dir;
};
