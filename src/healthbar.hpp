#pragma once

#include "common.hpp"
#include "gameobject.hpp"

//#include <vector>
//#include <array>
//#include <memory>

using namespace std;

class HealthBar : public GameObject
{

	static Texture m_health_texture, m_bar_texture;


public:
	HealthBar();
	~HealthBar();

	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	void draw_current(const mat3& projection, const mat3& current_transform)override;
	void draw_children(const mat3& projection, const mat3& current_transform)override;
	void update_current(float ms)override;
	void update_children(float ms)override;
	void set_percent_filled(float percent_filled) { m_percent_filled = percent_filled; }

	bool validate_textures();

private:
	float m_percent_filled;

};