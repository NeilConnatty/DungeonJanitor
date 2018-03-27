// gameobject.cpp

#include "gameobject.hpp"
#include <iostream>

GameObject::GameObject() :
	m_position({0.f, 0.f}),
	m_scale({1.f, 1.f}),
	m_size({1.f , 1.f}),
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

void GameObject::set_size(vec2 size)
{
	m_size = size;
}

void GameObject::set_rotation(float rotation)
{
	m_rotation = rotation;
}

vec2 GameObject::get_pos() const { return m_position; }
vec2 GameObject::get_scale() const { return m_scale; }
vec2 GameObject::get_size() const { return m_size; }
float GameObject::get_rot() const { return m_rotation; }
bool GameObject::is_enabled() const { return m_enabled; }


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
//Calculate and apply the force which will result in
//desired_vel being added to m_vel after dt
//could make it take m_vel into account as well.
void apply_force(vec2 desired_vel, float dt) {
	vec2 F = { 0, 0 };
	//dv/dt = a?
	//mda/dt = F
	//J = Integral(Fdt) from t1 to t2
	//mv = Fdt
	// F = mv/dt

	F.x = m_mass * desired_vel.x / dt;
	F.y = m_mass * desired_vel.y / dt;
	//Need to create a timer for each force...
	//A vector of floats? a vector of vec2's?
	//or a struct, returned by this method,
	//with the force vector and a float for the time
	//over which it is applied. 
	m_forces.x += F.x;
	m_forces.y += F.y;
}
//Calculate and apply the force which will result in
//change_in_position after dt
void apply_force(vec2 change_in_position, float dt) {

}

bool GameObject::collides_with(GameObject& object, mat3 room_transform, mat3 dungeon_transform) {
	float jLeftEdge = m_position.x;
	float jRightEdge = m_position.x+m_size.x;
	float jTopEdge = m_position.y;
	float jBottomEdge = m_position.y+m_size.y;

	float objX = get_world_coords_from_room_coords(object.get_pos(), room_transform, dungeon_transform).x;
	float objY = get_world_coords_from_room_coords(object.get_pos(), room_transform, dungeon_transform).y;

	float oLeftEdge =  objX;
	float oRightEdge =  objX+object.get_size().x;
	float oTopEdge =  objY;
	float oBottomEdge =  objY+object.get_size().y;

	if ((jLeftEdge <= oRightEdge && jRightEdge >= oLeftEdge) || (jRightEdge >= oLeftEdge && jLeftEdge <= oRightEdge))
	{
		if ((jTopEdge <= oBottomEdge && jBottomEdge >= oTopEdge) || (jBottomEdge >= oTopEdge && jTopEdge <= oBottomEdge))
		{
    		return true;
		}
	}
  return false;
}