#pragma once

#include "common.hpp"

class Floor : Renderable
{
	static Texture floor_texture;

public:
	Floor();
	~Floor();

	bool	init(vec2 position);
	void	destroy();
	
	void	set_position(vec2 position);
	void	draw(const mat3& projection) override;

private:
	vec2	m_position;
	vec2	m_scale;
};