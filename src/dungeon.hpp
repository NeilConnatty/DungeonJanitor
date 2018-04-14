#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"
#include "healthbar.hpp"
#include "emitter.hpp"
#include "boss.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

using namespace std;


class Dungeon : public GameObject
{
public:
	Dungeon();
	~Dungeon();
  
	bool init();
	void destroy();

	vector<unique_ptr<Room>>& get_rooms();
	bool add_doors(vector<std::unique_ptr<Door>>& doors);
	
	void add_adjacency(int roomID, Room::adjacent_room adj);
	vector<Room::adjacent_room>& get_adjacent_rooms(int roomID) { return m_adjacency_map.at(roomID); }
	string get_hero_timer();
	bool should_spawn_hero();
	bool hero_has_spawned();
	void spawn_hero();

	void setHealthBar(HealthBar* health_bar) { m_healthBar = health_bar; }
	
	void set_boss(Boss* boss);
	Boss* get_boss();
	void start_boss_fight();
	bool has_boss_fight_started();
	float get_boss_fight_dungeon_health();

private:
	void update_current(float ms) override;
	void update_children(float ms) override;
	void draw_current(const mat3 &projection,
                    const mat3 &current_transform) override;
	void draw_children(const mat3 &projection,
                     const mat3 &current_transform) override;

	float get_percent_dungeon_cleaned();

	//void test_value_iteration(); // for testing Jay

public:
	Room* janitor_start_room;
	Room* hero_start_room;
	Room* boss_start_room;
	vec2 janitor_room_position;
	vec2 hero_room_position;
	vec2 boss_room_position;
private:
	vector<std::unique_ptr<Room>> m_rooms;
	vector<std::unique_ptr<Door>> m_doors;
	unordered_map<int, vector<Room::adjacent_room>>     m_adjacency_map;
	float m_hero_timer;
	bool m_should_spawn_hero;
	bool m_hero_has_spawned;
	HealthBar* m_healthBar;
	vector<Emitter> m_emitters;
	Boss* m_boss;
	bool m_boss_fight_has_started;
};

