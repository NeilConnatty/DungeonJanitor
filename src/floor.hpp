#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class Floor : public GameObject
{
private:
	static Texture floor_textures[NUM_ROOM_TYPES];

public:
  static vec2 get_dimensions();

	Floor();
	~Floor();

	bool	init();
	bool	init(vec2 position, room_type type);
	void	destroy();
	
private:
	void update_current(float ms) override {} // no need for floor to be updated
	void update_children(float ms) override {}
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

private:
  room_type m_room_type;
};
