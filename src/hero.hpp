#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"

#include <vector>

class Hero : public GameObject
{

private:
	static Texture hero_texture;
	Room * m_currentRoom;
	vec2 m_destination;
	vec2 m_vel;

public:
	Hero();
	~Hero();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	void setRoom(Room * room);
	void set_destination(vec2 position);
	void reset_destination();

private:
	void update_current(float ms) override;
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
	void calculate_room_rewards();
	Room * calculate_best_neighbor_room(Room * room);
};