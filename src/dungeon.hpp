#pragma once

#include "common.hpp"
#include "room.hpp"

#include <vector>

class Dungeon : Renderable
{
public:
	Dungeon();
	~Dungeon();

	bool init();
	void destroy();

	void set_position(vec2 pos);
	void update(float ms);
	void draw(const mat3& projection, const mat3& parent_transform) override;

private:
	void update_current(float ms);
	void update_children(float ms);

private:
	std::vector<Room::ptr>	m_rooms;  // unsure if this should be a vector of pointers or rooms
	vec2					m_position;
};