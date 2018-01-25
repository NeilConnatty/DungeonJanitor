#pragma once

#include "common.hpp"

class Wall : Renderable
{
	static Texture wall_texture;

public:
	Wall();
	~Wall();

	bool	init();
	void	destroy();

	void	set_position(vec2 position);
	vec2	get_bounding_box() const;
	void	draw(const mat3& projection) override;

private:
	vec2	m_position;
	vec2	m_scale;
};