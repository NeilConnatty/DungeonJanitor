#pragma once

#include "common.hpp"

class GameObject : public Renderable
{
public:
	GameObject();
	~GameObject();

	void set_position(vec2 pos);
	void set_scale(vec2 scale);
	void set_rotation(float rotation);

	void update(float ms);
	void draw(const mat3& projection, const mat3& parent_transform) override;

protected:
	virtual void update_current(float ms) = 0;
	virtual void update_children(float ms) = 0;
	virtual void draw_current(const mat3& projection, const mat3& current_transform) = 0;
	virtual void draw_children(const mat3& projection, const mat3& current_transform) = 0;

protected:
	vec2	m_position;
	vec2	m_scale;
	float	m_rotation;
};