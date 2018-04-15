#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"
#include "dungeon.hpp"
#include "pathfinder.hpp"
#include "ValueIteration.hpp"
#include <vector>

class Hero : public GameObject
{
public:
	enum destinations
	{
		DOOR = 0,
		ARTIFACT = 1,
		BOSS = 2
	};
	enum direction {
		down, left, right, up
	};
private:
	bool m_is_moving;
	bool m_is_at_boss;
	float m_artifact_offset;

	const float SPEED = 100.0f;
	//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
	const float Y_SPEED = SPEED * (24.f / 35.f);

	vec2 m_end_destination;
	vec2 m_current_destination;
	vec2 m_vel;
	vector<vec2> m_path;

	Hero::destinations m_destination_type;
	
	static Texture hero_texture;
	
	Room * m_currentRoom;
	Room* m_next_room;
	vector<unique_ptr<Room>>* m_rooms;
	Dungeon* m_dungeon;	
	

public:
	Hero();
	~Hero();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	void setRoom(Room * room);
	void setDungeon(Dungeon* dungeon);
	void setAllRooms(vector<unique_ptr<Room>>* rooms);
	void set_destination(vec2 position, Hero::destinations destination);
	void stop_movement();
	void update_path();
	bool is_moving();
	bool is_at_boss();
	const Room* get_current_room();

	vec2 get_next_door_position(); // which door to go to next, returned in world coordinates

private:
	void update_current(float ms) override;
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
	
	void pick_movement_tex();

	int frame;
	int const NUM_FRAMES = 4;
	float animation_frame_w;
	float animation_frame_h;
	direction animation_dir;
	float m_time_elapsed;
	float const MS_PER_FRAME = (1 / 12.5) * 1000;
};
