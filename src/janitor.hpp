#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "wall.hpp"

class Janitor : public GameObject
{
	static Texture placeholder_texture;
public:
	Janitor();
	~Janitor();

// Creates all the associated render resources and default transform
	bool init();
	bool init(vec2 position);
// Releases all associated resources
	void destroy();

	void key_up();
	void key_down();
	void key_left();
	void key_right();

	void set_accel(vec2 newAccel);
	void set_vel(vec2 newVel);

	bool collides_with(Wall& wall);


	void update_current(float ms)override;
	void update_children(float ms)override;
	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;

private:
	//some acceleration value until you reach a max speed
	vec2 m_accel;
	vec2 m_vel;

	bool m_key_up;
	bool m_key_down;
	bool m_key_left;
	bool m_key_right;

};
