#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"

#include <vector>

class Dungeon : public GameObject
{
public:
	Dungeon();
	~Dungeon();

	bool init();
	void destroy();

private:
	void update_current(float ms) override;
	void update_children(float ms) override;
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
	std::vector<Room::ptr>	m_rooms;  // unsure if this should be a vector of pointers or rooms
};