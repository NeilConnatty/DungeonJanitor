#pragma once

#include "../common.hpp"
#include "../gameobject.hpp"

#include <vector>

class Puddle : public GameObject
{

private:
	static Texture puddle_texture;

public:
	Puddle();
	~Puddle();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	
private:
	void update_current(float ms) override {};
	void update_children(float ms) override {}; // Puddles don't have children 
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};