#pragma once

#include "common.hpp"
#include "gameobject.hpp"

#include <vector>

class Janitor : public GameObject
{
	static Texture up1, up2, up3, up4,
		up_right1, up_right2, up_right3, up_right4,
		up_left1, up_left2, up_left3, up_left4,
		down1, down2, down3, down4,
		down_right1, down_right2, down_right3, down_right4,
		down_left1, down_left2, down_left3, down_left4,
		left1, left2, left3, left4, 
		right1, right2, right3, right4;
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
	enum DIRECTION {
		up, up_right, up_left, down, down_right, down_left, left, right
	};
	bool validate_textures();
	void pick_movement_tex(DIRECTION dir, const int FRAME_TIMING);
	void set_accel(vec2 newAccel);
	void set_vel(vec2 newVel);

	bool collides_with(GameObject& object, mat3 room_transform, mat3 dungeon_transform);

	void update_current(float ms)override;
	void update_children(float ms)override;
	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;
private:
	vec2 m_accel;
	vec2 m_vel;

	//Potentially make this a vector of pointers later.
	Texture* m_curr_tex;
	float m_time_pressed;
	bool m_key_up;
	bool m_key_down;
	bool m_key_left;
	bool m_key_right;
};
