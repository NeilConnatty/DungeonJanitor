#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "door.hpp"
#include "Cleanable/puddle.hpp"
#include "Cleanable/graffiti.hpp"
#include "Cleanable/artifact.hpp"
#include "Cleanable/cleanable.hpp"
#include "Cleanable/garbage.hpp"
#include "floorobject.hpp"

#include <vector>
#include <array>
#include <memory>
#include <random>

using namespace std;

class Janitor;

class Room : public GameObject
{
public: 
  using wall_pair = pair<vec2, wall_edge>;

  struct adjacent_room
  {
    Room* room;
    Door* door;
  };

  enum directions
  {
    NORTH = 0,
    SOUTH = 1,
    EAST  = 2,
    WEST  = 3
  };

public:
    Room();
    ~Room();

    bool init();
    bool init(vec2 position, room_type type);
    void destroy();

    bool add_wall(wall_pair wall);
    bool add_floor(vec2 floor);
    bool add_walls(vector<wall_pair>& walls);
    bool add_floors(vector<vec2>& positions);
    bool add_cleanables(vector<pair<Cleanable::types, vec2>>& cleanable_pos);
    bool add_artifact(bool has_artifact, vec2 artifact_pos);
    bool add_hero_spawn_loc(bool has_hero_spawn, vec2 hero_spawn_pos);
    bool add_boss_spawn_loc(bool has_boss_spawn, vec2 boss_spawn_pos);
    bool add_janitor_spawn_loc(bool has_janitor_spawn, vec2 janitor_spawn_pos);

    vector<Wall>& get_walls();
	  vector<unique_ptr<Cleanable>>&  get_cleanables();
    vector<unique_ptr<FloorObject>>& get_floor_objects();

    
    void setReward(double reward);
	double getReward() const;

    bool has_hero_spawn_loc() const;
    bool has_boss_spawn_loc() const;
	bool has_janitor_spawn_loc() const;
	bool containsBoss() const;
	bool containsArtifact();

	vec2 get_hero_spawn_loc() const;
	vec2 get_boss_spawn_loc() const;
    vec2 get_janitor_spawn_loc() const;
    
    void setBossInRoom(bool bossInRoom);
    void deactivate_artifact();
	Artifact* get_artifact();
	void set_hero_has_visited(bool visited);
	bool has_hero_visited();

	void clean(Janitor* janitor, mat3 dungeon_transform);
	void increment_cleaned_cleanables() { m_num_cleaned_cleanables++; } // Dana -- needs to be better plugged in (currently plugged into World)
	void increment_activated_artifacts() { m_num_activated_artifacts++; } // Dana -- ditto (currently plugged into Dungeon)
	int get_number_total_cleanables() { return m_total_cleanables; }
	int get_number_cleaned_cleanables() { return m_num_cleaned_cleanables; }
	int get_number_total_artifacts() { return m_total_artifacts; }
	int get_number_activated_artifacts() { return m_num_activated_artifacts; }
	int get_number_spawned_boss_cleanables() { return m_num_spawned_boss_fight_cleanables; }
	int get_number_cleaned_boss_cleanables() { return m_num_cleaned_boss_fight_cleanables; }

    void set_room_type(room_type type);
    bool populate_floor_objects();
    int getRoomID() const { return m_ID; };
    void setRoomID(int id);
	void setDungeonTransform(mat3 transform) { m_dungeon_transform = transform; }
	mat3 getDungeonTransform() { return m_dungeon_transform; }

	void spawn_debris();

private:
    void update_current(float ms) override;
    void update_children(float ms) override;
    void draw_current(const mat3& projection, const mat3& current_transform) override;
    void draw_children(const mat3& projection, const mat3& current_transform) override;

private:

    bool m_BossHere; // update as Boss moves
    bool m_ArtifactHere; // update when artifact a) fixed by player or b) discovered by hero
    bool m_has_hero_spawn_loc;
    bool m_has_boss_spawn_loc;
    bool m_has_janitor_spawn_loc;
	bool m_hero_has_visited;

    int m_ID; // unique room id
    int m_num_cleaned_cleanables; 
    int m_total_cleanables;
	int m_num_activated_artifacts;
	int m_total_artifacts;
    double heroRewardValue;

	int m_num_spawned_boss_fight_cleanables;
	int m_num_cleaned_boss_fight_cleanables;


    vec2 m_hero_spawn_loc;
    vec2 m_boss_spawn_loc;
    vec2 m_janitor_spawn_loc;

	mat3 m_dungeon_transform;
    
    Artifact		m_artifact;

    room_type m_room_type;

    vector<Floor>		          m_floors;
    vector<Wall>		          m_walls;
	vector<unique_ptr<Cleanable>>			  m_cleanables;
	vector<unique_ptr<FloorObject>>     m_floor_objects;
}; 

