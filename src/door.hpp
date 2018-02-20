#pragma once

#include "common.hpp"
#include "door.hpp"
#include "gameobject.hpp"
#include "floor.hpp"

#include <array>

class Door : public GameObject
{
public:
  Door();
  ~Door();

  bool init() override { return init({ 0.f, 0.f }); }
  bool init(vec2 pos);
  void destroy() override;

protected:
  void update_current(float ms) override;
  void update_children(float ms) override;
  void draw_current(const mat3& projection, const mat3& current_transform) override;
  void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
  std::array<Floor, 3> m_floors;
};