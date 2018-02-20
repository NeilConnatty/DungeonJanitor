// hallway.cpp

#include "hallway.hpp"
#include "roomtextparser.hpp"

bool Hallway::init(vec2 pos)
{
  RoomParser parser;
  m_room.init();
  
  if (!parser.parseRoom(m_room, room_path("hallway.rm")))
  {
    fprintf(stderr, "Failed to parse hallway.\n");
    return false;
  }

  m_position = pos;

  return true;
}

void Hallway::destroy()
{
  m_room.destroy();
}

void Hallway::update_children(float ms)
{
  m_room.update(ms);
}

void Hallway::draw_children(const mat3& projection, const mat3& current_transform)
{
  m_room.draw(projection, current_transform);
}
