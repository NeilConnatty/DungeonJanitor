#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class StartScreen :public GameObject
{
	static Texture startscreen1;
	static Texture startscreen2;
public:
	StartScreen();
	~StartScreen();
	bool init();
	bool init(vec2 pos);
	void destroy();
private:
	float m_animation_time;
	Texture* m_curr_tex;
	void update_current(float ms)override;
	void update_children(float ms)override;
	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;
};