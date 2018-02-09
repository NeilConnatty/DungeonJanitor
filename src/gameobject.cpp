// gameobject.cpp

#include "gameobject.hpp"

GameObject::GameObject() :
	m_position({0.f, 0.f}),
	m_scale({1.f, 1.f}),
	m_rotation(0.f),
	m_enabled(true)
{
}

GameObject::~GameObject()
{
}

void GameObject::set_pos(vec2 pos)
{
	m_position = pos;
}

void GameObject::set_scale(vec2 scale)
{
	m_scale = scale;
}

void GameObject::set_rotation(float rotation)
{
	m_rotation = rotation;
}


vec2 GameObject::get_pos() { return m_position; }
vec2 GameObject::get_scale() { return m_scale; }
float GameObject::get_rot() { return m_rotation; }
bool GameObject::is_enabled() { return m_enabled; }

void GameObject::toggle_enable()
{
	m_enabled = !m_enabled;
}


void GameObject::update(float ms)
{
	if (m_enabled) 
	{
		update_current(ms);
		update_children(ms);
	}
}

void GameObject::draw(const mat3& projection, const mat3& parent_transform)
{
	if (m_enabled)
	{
		// Transformation code, see Rendering and Transformation in the template specification for more info
		// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
		transform_begin();
		transform_translate(m_position);
		transform_rotate(m_rotation);
		transform_scale(m_scale);
		transform_end();

		mat3 final_transform = parent_transform * transform;

		draw_current(projection, final_transform);
		draw_children(projection, final_transform);
	}
}