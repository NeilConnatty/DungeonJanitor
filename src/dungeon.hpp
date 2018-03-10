#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;


class Dungeon : public GameObject
{
public:
  Dungeon();
  ~Dungeon();
  
  bool init();
  void destroy();
  void clean();
  void activate_artifact();
  vector<unique_ptr<Room>>& get_rooms();
  bool add_doors(vector<vec2>& door_pos, int roomID);

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
	vector<std::unique_ptr<Room>> m_rooms;
  vector<std::unique_ptr<Door>> m_doors;
  unordered_map<int, Door*>     m_door_map;
};

