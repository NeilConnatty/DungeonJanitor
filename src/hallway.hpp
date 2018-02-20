#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"

class Hallway : public GameObject
{
public:
  bool init() override;
  void destroy() override;

protected:
  virtual void update_current(float ms) override {}
  virtual void update_children(float ms) override;
  virtual void draw_current(const mat3& projection, const mat3& current_transform) override {}
  virtual void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
  Room m_room;
};