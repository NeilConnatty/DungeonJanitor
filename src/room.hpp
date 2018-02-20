#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "floor.hpp"
#include "wall.hpp"
#include "Cleanable/puddle.hpp"
#include "Cleanable/artifact.hpp"

#include <vector>
#include <memory>

using namespace std;

class Room : public GameObject
{
public: 
  using wall_pair = pair<vec2, wall_edge>;

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
	bool add_cleanables(vector<vec2>& puddle_positions);
	bool add_artifact(bool has_artifact, vec2 artifact_pos);

	vector<Puddle>&  get_cleanables();

	double getReward();
	void setReward(double reward);

	bool containsBoss();
	bool containsArtifact();
	void setBossInRoom(bool bossInRoom);
	void setArtifactInRoom(bool artifactInRoom);
	
	Room* getNorthRoom();
	Room* getSouthRoom();
	Room* getWestRoom();
	Room* getEastRoom();

	void setNorthRoom(Room* room);
	void setSouthRoom(Room* room);
	void setEastRoom(Room* room);
	void setWestRoom(Room* room);

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
	int m_ID; // unique room id
	double heroRewardValue;
	Room * m_NorthRoom, * m_SouthRoom, * m_WestRoom, * m_EastRoom; // Can Be Nullptr
	vector<Floor>		m_floors;
	vector<Wall>		  m_walls;
	vector<Puddle>   m_puddles;
	Artifact		m_artifact;
}; 
