#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "door.hpp"
#include "Cleanable/puddle.hpp"
#include "Cleanable/artifact.hpp"

#include <vector>
#include <array>
#include <memory>

using namespace std;

class Room : public GameObject
{
public: 
  using wall_pair = pair<vec2, wall_edge>;

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
    bool init(vec2 position);
    void destroy();

    bool add_wall(wall_pair wall);
    bool add_floor(vec2 floor);
    bool add_walls(vector<wall_pair>& walls);
    bool add_floors(vector<vec2>& positions);
	bool add_door(vec2 position);
	bool add_doors(std::vector<vec2>& positions);
    bool add_cleanables(vector<vec2>& puddle_positions);
    bool add_artifact(bool has_artifact, vec2 artifact_pos);
    bool add_hero_spawn_loc(bool has_hero_spawn, vec2 hero_spawn_pos);
    bool add_boss_spawn_loc(bool has_boss_spawn, vec2 boss_spawn_pos);
    bool add_janitor_spawn_loc(bool has_janitor_spawn, vec2 janitor_spawn_pos);

    vector<Puddle>&  get_cleanables();

    double getReward();
    void setReward(double reward);
    bool has_hero_spawn_loc();
    vec2 get_hero_spawn_loc();
    bool has_boss_spawn_loc();
    vec2 get_boss_spawn_loc();
    bool has_janitor_spawn_loc();
    vec2 get_janitor_spawn_loc();
    bool containsBoss();
    bool containsArtifact();
    void setBossInRoom(bool bossInRoom);
    void setArtifactInRoom(bool artifactInRoom);

    int get_num_cleanables();
    float get_clean_percent();
    void decrement_cleanables();
    
    Room* get_north_room();
    Room* get_south_room();
    Room* get_east_room();
    Room* get_west_room();

	vector<Door>* get_m_doors(); // temp work around to access doors to in order to set them with the room, hopefully this can later be handled by dungeon parser
	// This probably wont work when we add more rooms to the dungeon

    void  set_north_room(Room* rm, Door* d);
    void  set_south_room(Room* rm, Door* d);
    void  set_east_room(Room* rm, Door* d);
    void  set_west_room(Room* rm, Door* d);

    int getRoomID();
    void setRoomID(int id);

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

    int m_ID; // unique room id
    int m_num_cleanables;
    int m_total_cleanables;
    double heroRewardValue;

    vec2 m_hero_spawn_loc;
    vec2 m_boss_spawn_loc;
    vec2 m_janitor_spawn_loc;
    
    Room * m_NorthRoom, * m_SouthRoom, * m_WestRoom, * m_EastRoom; // Can Be Nullptr
    Artifact		m_artifact;

    vector<Floor>		m_floors;
    vector<Wall>		  m_walls;
    vector<Puddle>   m_puddles;
    vector<Door>     m_doors;
    array<Room*, 4>   m_adjacent_rooms; // implemented as array so that it can be iterated through
	array<Door*, 4> m_adjacent_doors; // correponds with m_adjacent_rooms
}; 

