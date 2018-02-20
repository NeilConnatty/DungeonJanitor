// door.cpp

#include "door.hpp"

Door::Door() {}
Door::~Door() {}

bool Door::init(vec2 pos)
{
  for (Floor floor : m_floors)
  {
    if (!floor.init())
    {
      fprintf(stderr, "failed to init Floor.\n");
      return false;
    }
  }

  m_position = pos;
  m_floors[1].set_pos({ 0.f, 25.f });
  m_floors[2].set_pos({ 0.f, 50.f });

  return true;
}

void Door::destroy()
{
  for (Floor floor : m_floors)
  {
    floor.destroy();
  }
}

void Door::update_current(float ms) {};
void Door::update_children(float ms) {}
void Door::draw_current(const mat3& projection, const mat3& current_transform) {}

void Door::draw_children(const mat3& projection, const mat3& current_transform)
{
  for (Floor floor : m_floors)
  {
    floor.draw(projection, current_transform);
  }
}
