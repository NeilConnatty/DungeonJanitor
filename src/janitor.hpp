#pragma once

#include "common.hpp"
#include "spritesheet.hpp"
#include "gameobject.hpp"

#include <vector>

class Janitor : public GameObject
{
	static Texture up1, up2, up3, up4, down1, down2, down3, down4, left1, left2, left3, left4, right1, right2, right3, right4;
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

	//bool collides_with(GameObject& object);

	void move(vec2 offset);

	void update_current(float ms)override;
	void update_children(float ms)override;
	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;
private:
	//Spritesheet m_up_sheet;
	vec2 m_accel;
	vec2 m_vel;
	vec2 m_curr_tex;

	//std::vector<Texture> m_tex_sheet;
	Texture* m_tex_sheet;
	int m_tex_index;
	
	int m_key_cycles;

	bool m_key_up;
	bool m_key_down;
	bool m_key_left;
	bool m_key_right;

};
