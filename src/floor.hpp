#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class Floor : public GameObject
{
private:
	static Texture floor_texture;

public:
	Floor();
	~Floor();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	
private:
	void update_current(float ms) override {} // no need for floor to be updated
	void update_children(float ms) override {}
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;
};
