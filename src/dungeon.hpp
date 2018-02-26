#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"

#include <vector>
using namespace std;

class Dungeon : public GameObject
{
public:
  Dungeon();
  ~Dungeon();
  
  bool init();
  void destroy();
  void clean(vec2 janitor_pos);
  vector<Room> get_rooms();

private:
  void update_current(float ms) override;
  void update_children(float ms) override;
  void draw_current(const mat3 &projection,
                    const mat3 &current_transform) override;
  void draw_children(const mat3 &projection,
                     const mat3 &current_transform) override;

	void test_value_iteration(); // for testing Jay

public:
	Room * janitor_start_room;
	Room* hero_start_room;
	Room* boss_start_room;
	vec2 janitor_room_position;
	vec2 hero_room_position;
	vec2 boss_room_position;
private:
	vector<Room>	m_rooms;  // unsure if this should be a vector of pointers or rooms
};

