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

private:
	static Texture hero_texture;
	Room * m_currentRoom;
	Dungeon* m_dungeon;
	Room* m_next_room;
	Hero::destinations m_destination_type;
	vec2 m_end_destination;
	vec2 m_current_destination;
	vector<vec2> m_path;
	vec2 m_vel;
	bool m_is_moving;
	vector<unique_ptr<Room>>* m_rooms; 
	const float SPEED = 100.0f;
	//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
	const float Y_SPEED = SPEED * (24.f / 35.f);

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
	const Room* get_current_room();

	vec2 get_next_door_position(); // which door to go to next

private:
	void update_current(float ms) override;
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
