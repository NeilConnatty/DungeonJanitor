#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class Winscreen :public GameObject
{
	static Texture win_screen_texture;
public:
	Winscreen();
	~Winscreen();
	bool init();
	bool init(vec2 pos);
	void destroy();
private:
	void update_current(float ms)override;
	void update_children(float ms)override;
	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;
};