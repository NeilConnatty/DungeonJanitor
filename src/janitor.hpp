#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "room.hpp"
#include "dungeon.hpp"

#include <vector>

class Dungeon;

class Janitor : public GameObject
{
	static Texture janitor_sheet;
public:
	Janitor();
	~Janitor();

// Creates all the associated render resources and default transform
	bool init();
	bool init(vec2 position);
// Releases all associated resources
	void destroy();

	void key_up(bool move);
	void key_down(bool move);
	void key_left(bool move);
	void key_right(bool move);
	enum DIRECTION {
		right, up_right, up, up_left, left, down_left, down, down_right
	};
	bool validate_textures();
	void pick_movement_tex();
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

	DIRECTION animation_dir;
	float m_time_elapsed;
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

	int frame;
	int const NUM_FRAMES = 4;
	float animation_frame_w;
	float animation_frame_h;
	float const MS_PER_FRAME = (1 / 12.5)*1000;
};
