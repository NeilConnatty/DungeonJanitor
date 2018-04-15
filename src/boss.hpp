#pragma once

#include "common.hpp"
#include "gameobject.hpp"

#include <vector>

class Boss : public GameObject
{

private:
	static Texture boss_texture;
	enum direction {
		right, left
	};
public:
	Boss();
	~Boss();

	bool	init();
	bool	init(vec2 position);
	void	destroy();

private:
	void update_current(float ms) override;
	void update_children(float ms) override {}; 
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
	void pick_movement_tex();

	float m_time_elapsed;
	int animation_dir;
	int frame;
	int const NUM_FRAMES = 4;
	float animation_frame_w;
	float animation_frame_h;
	float const MS_PER_FRAME = (1 / 12.5) * 1000;
};
