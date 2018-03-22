#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"
#include "dungeon.hpp"

#include <vector>

class Dungeon;

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
  bool collides_with_door(Door& door, mat3 room_transform, mat3 dungeon_transform);

	void key_up(bool move);
	void key_down(bool move);
	void key_left(bool move);
	void key_right(bool move);
	enum DIRECTION {
		up, up_right, up_left, down, down_right, down_left, left, right
	};
	bool validate_textures();
	void pick_movement_tex(const int FRAME_TIMING);
	void set_accel(vec2 newAccel);
	void set_vel(vec2 newVel);

	void set_dungeon(Dungeon* dungeon);

	int get_current_room_id();
	void set_current_room(Room* room);
	Room* get_current_room();

	void check_collisions();
	void check_movement();

	void update_current(float ms)override;
	void update_children(float ms)override;
	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;
private:
	Room* m_currentRoom;
	Dungeon* m_dungeon;
	vec2 m_accel;
	vec2 m_vel;

	//Potentially make this a vector of pointers later.
	Texture* m_curr_tex;
	float m_animation_time;
	float m_time_pressed_up;
	float m_time_pressed_down;
	float m_time_pressed_left;
	float m_time_pressed_right;
	bool m_key_up;
	bool m_key_down;
	bool m_key_left;
	bool m_key_right;
	bool can_move_up;
 	bool can_move_down;
 	bool can_move_left;
 	bool can_move_right;
	bool m_door_collision_last_frame;
};
