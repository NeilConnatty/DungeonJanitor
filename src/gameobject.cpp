// gameobject.cpp

#include "gameobject.hpp"
#include <iostream>

GameObject::GameObject() :
	m_position({ 0.f, 0.f }),
	m_scale({ 1.f, 1.f }),
	m_size({ 1.f , 1.f }),
	m_rotation(0.f),
	m_external_force({ 0, 0 }),
	m_accel({ 0, 0 }),
	m_vel({ 0, 0 }),
	m_mass(1.f),
	m_enabled(true),
	m_active_forces(ForceList())
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
//destroy/reinit the the active_force list as well
void GameObject::toggle_physics() {
	m_physics_object = !m_physics_object;
}

void GameObject::update(float ms)
{
	if (m_enabled) 
	{
		update_current(ms);
		update_children(ms);
	}
	if (m_physics_object && m_active_forces.size != 0) {
		int i = 0;
		ForceList::ActiveForce* af = m_active_forces.head;
		while (af->next != nullptr) {
			if (!af->applied) {
				m_external_force.x += af->force.x;
				m_external_force.y += af->force.y;
				af->applied = true;
			}
			af->tick(ms);
			//consider tapering them over time.
			if (af->dt < 0) {
				m_external_force.x -= af->force.x;
				m_external_force.y -= af->force.y;
				af->force = { 0,0 };
				ForceList::ActiveForce* temp = af;
				af = af->next;
				if (temp == m_active_forces.head) {
					m_active_forces.delete_head();
				}
				//probably has pointer errors
				else {
					m_active_forces.delete_pos(i);
				}
			}
			else
				af = af->next;
			if (af == nullptr) return;
			i++;
		}
		//tail check.
		if (af->next == nullptr) {
			if (!af->applied) {
				m_external_force.x += af->force.x;
				m_external_force.y += af->force.y;
				af->applied = true;
			}
			af->tick(ms);
			if (af->dt < 0) {
				m_external_force.x -= af->force.x;
				m_external_force.y -= af->force.y;
				af->force = { 0,0 };
				m_active_forces.delete_tail();
			}
		}
		if (m_external_force.x > -1 && m_external_force.x < 1) m_external_force.x = 0;
		if (m_external_force.y > -1 && m_external_force.y < 1) m_external_force.y = 0;
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
//desired_vel is added to m_vel by a constant force over dt in milliseconds
void GameObject::apply_force_dv(vec2 desired_vel, float dt)
{
	vec2 F = { 0, 0 };
	float time_factor = dt/1000.f;
	F.x = m_mass * (desired_vel.x / time_factor);
	F.y = m_mass * (desired_vel.y / time_factor);
	m_active_forces.insert_back(ForceList::ActiveForce(F, dt));
}
//Calculate and apply the force which will result in
//change_in_position after dt
void GameObject::apply_force_dx(vec2 dx, float dt) 
{
	vec2 F = { 0, 0 };
	float time_factor = dt/1000.f;
	float dvx = dx.x / time_factor;
	float dvy = dx.y / time_factor;

	//J = mv2 - mv1
	//J/dt = average force over dt
	F.x = (m_mass * dvx - m_mass * m_vel.x) / time_factor;
	F.y = (m_mass * dvy - m_mass * m_vel.y) / time_factor;

	m_active_forces.insert_back(ForceList::ActiveForce(F, dt));
}

bool GameObject::collides_with(GameObject& object, mat3 room_transform, mat3 dungeon_transform) {
	float jLeftEdge = m_position.x;
	float jRightEdge = m_position.x+m_size.x;
	float jTopEdge = m_position.y;
	float jBottomEdge = m_position.y+m_size.y;

	float objX = get_world_coords_from_room_coords(object.get_pos(), room_transform, dungeon_transform).x;
	float objY = get_world_coords_from_room_coords(object.get_pos(), room_transform, dungeon_transform).y;

	float oLeftEdge =  objX;
	float oRightEdge =  objX+object.get_size().x * object.get_scale().x;
	float oTopEdge =  objY;
	float oBottomEdge =  objY+object.get_size().y * object.get_scale().y;

	if ((jLeftEdge <= oRightEdge && jRightEdge >= oLeftEdge) || (jRightEdge >= oLeftEdge && jLeftEdge <= oRightEdge))
	{
		if ((jTopEdge <= oBottomEdge && jBottomEdge >= oTopEdge) || (jBottomEdge >= oTopEdge && jTopEdge <= oBottomEdge))
		{
    		return true;
		}
	}
  return false;
}


bool GameObject::collides_with_projected(GameObject & object, vec2 projected_position, mat3 room_transform, mat3 dungeon_transform)
{
	float jLeftEdge = projected_position.x;
	float jRightEdge = jLeftEdge + m_size.x;
	float jTopEdge = projected_position.y;
	float jBottomEdge = jTopEdge + m_size.y;


	float objX = get_world_coords_from_room_coords(object.get_pos(), room_transform, dungeon_transform).x;
	float objY = get_world_coords_from_room_coords(object.get_pos(), room_transform, dungeon_transform).y;

	float oLeftEdge = objX;
	float oRightEdge = objX + object.get_size().x ;
	float oTopEdge = objY;
	float oBottomEdge = objY + object.get_size().y;

	if (object.is_enabled() && ((jLeftEdge <= oRightEdge && jRightEdge >= oLeftEdge) || (jRightEdge >= oLeftEdge && jLeftEdge <= oRightEdge)))
	{
		if ((jTopEdge <= oBottomEdge && jBottomEdge >= oTopEdge) || (jBottomEdge >= oTopEdge && jTopEdge <= oBottomEdge))
		{
			//object.toggle_enable(); //For Debug
			return true;
		}
	}
	return false;
}

