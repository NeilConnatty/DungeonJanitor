#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"
#include "ValueIteration.hpp"
#include <vector>

class Hero : public GameObject
{

private:
	static Texture hero_texture;
	Room * m_currentRoom;
	vec2 m_destination;
	vec2 m_vel;
	bool m_is_moving;

public:
	Hero();
	~Hero();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	void setRoom(Room * room);
	void set_destination(vec2 position);
	void stop_movement();

private:
	void update_current(float ms) override;
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
	
	vec2 get_next_door_position();
};
