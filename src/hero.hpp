#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"
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
	Room* m_next_room;
	Hero::destinations m_destination_type;
	vec2 m_destination;
	vec2 m_vel;
	bool m_is_moving;
	vector<unique_ptr<Room>>* m_rooms;

public:
	Hero();
	~Hero();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	void setRoom(Room * room);
	void setAllRooms(vector<unique_ptr<Room>>* rooms);
	void set_destination(vec2 position, Hero::destinations destination);
	void stop_movement();
	bool is_moving();
	Room* get_current_room();

	vec2 get_next_door_position(); // which door to go to next

private:
	void update_current(float ms) override;
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
