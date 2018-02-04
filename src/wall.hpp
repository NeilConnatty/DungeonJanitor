#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class Wall : public GameObject
{
	static Texture wall_texture;

public:
	Wall();
	~Wall();

	bool	init();
	bool	init(vec2 position);
	void	destroy();

	vec2	get_bounding_box() const;

private:
	void update_current(float ms) override {} // no need for wall to be updated
	void update_children(float ms) override {}
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;
};
