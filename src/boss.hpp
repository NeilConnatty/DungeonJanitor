#pragma once

#include "common.hpp"
#include "gameobject.hpp"

#include <vector>

class Boss : public GameObject
{

private:
	static Texture boss_texture;

public:
	Boss();
	~Boss();

	bool	init();
	bool	init(vec2 position);
	void	destroy();

private:
	void update_current(float ms) override {};
	void update_children(float ms) override {}; 
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
