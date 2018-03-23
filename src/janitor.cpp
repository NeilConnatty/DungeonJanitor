#include "janitor.hpp"
#include "wall.hpp"
#include "dungeon.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

//This is ugly af
Texture Janitor::janitor_sheet;

Janitor::Janitor() : GameObject()
{
}

Janitor::~Janitor() {}

//Init graphics resources
bool Janitor::init() { return false; }
bool Janitor::init(vec2 position)
{
	if (!validate_textures()) return false;
	//scale of the texture and resulting mesh
	float wr = janitor_sheet.width/4.f * 0.5f;
	float hr = janitor_sheet.height/8.f * 0.5f;
	animation_frame_w = 1 / 4.0f;
	animation_frame_h = 1 / 8.0f;
	//texcoords are in the range [0..1]
	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, animation_frame_h };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { animation_frame_w, animation_frame_h };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { animation_frame_w, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f,  0.f};

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("animated.vs.glsl"), shader_path("animated.fs.glsl")))
		return false;

	//Initialize member variables
	m_position = position;
	m_size = {static_cast<float>(janitor_sheet.width/4), static_cast<float>(janitor_sheet.height/8)};
	animation_dir = right;
	frame = 0;
	m_time_elapsed = 0;
	m_time_pressed_up = 0;
	m_time_pressed_down = 0;
	m_time_pressed_left = 0;
	m_time_pressed_right = 0;
	m_key_up = false;
	m_key_down = false;
	m_key_left = false;
	m_key_right = false;
	m_door_collision_last_frame = false;
	return true;
}

//Destroy graphics resources
void Janitor::destroy() 
{
  glDeleteBuffers(1, &mesh.vbo);
  glDeleteBuffers(1, &mesh.ibo);
  glDeleteBuffers(1, &mesh.vao);

  glDeleteShader(effect.vertex);
  glDeleteShader(effect.fragment);
  glDeleteShader(effect.program);
}

void Janitor::update_current(float ms) 
{
	vec2 force_sum = { 0, 0 };
	vec2 movement_force = { 0, 0 };
	vec2 friction = { 0, 0 };

	//Starts high but quickly tapers down
	float const BUTTON_FORCE = 800.f;
	//Friction constant of concrete
	float const FRICTIONAL_CONST = 0.6;
	float const MASS = 60.f;
	float const G = 9.81;
	float friction_scalar = FRICTIONAL_CONST * MASS * G;
	friction_scalar = friction_scalar*friction_scalar;
	/* MIN_VEL is holdover, currently it has no effect, but it may be useful later, please leave for now*/
	float const MIN_VEL = 0.f;
	float const MAX_VEL = 220.f;
	float const MAX_ACCEL = 600.f;

	//const float SPEED = 180.0f;
	//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
	//const float Y_SPEED = SPEED * (24.f / 35.f);
	float time_factor = ms / 1000;

	m_time_elapsed += ms;
	if(m_key_up || m_key_down || m_key_right || m_key_left)
		if (m_time_elapsed > MS_PER_FRAME) {
			m_time_elapsed = 0;
			frame = (frame + 1) % NUM_FRAMES;
		}
	check_collisions();
	//UP
	if (m_key_up)
	{
		m_time_pressed_up += ms;
		movement_force.y = (BUTTON_FORCE - (m_time_pressed_up*m_time_pressed_up));
		if (abs(movement_force.y) < friction_scalar)
			movement_force.y = -friction_scalar;
		//if (m_vel.y > -MIN_VEL) m_vel.y = -MIN_VEL;
	}
	else m_time_pressed_up = 0;
	//DOWN
	if (m_key_down)
	{
		m_time_pressed_down += ms;
		movement_force.y = -(BUTTON_FORCE - (m_time_pressed_down*m_time_pressed_down));
		if (abs(movement_force.y) < friction_scalar)
			movement_force.y = friction_scalar;
		//if (m_vel.y < MIN_VEL) m_vel.y = MIN_VEL;
	}
	else m_time_pressed_down = 0;
	if (!m_key_up && !m_key_down)
	{
		if ((m_vel.y > 0 && m_vel.y < 20.f) || m_vel.y < 0 && m_vel.y > -20.f) {
			m_vel.y = 0;
			m_accel.y = 0;
		}
	}
	//LEFT
	if (m_key_left)
	{
		m_time_pressed_left += ms;
		movement_force.x = (BUTTON_FORCE - (m_time_pressed_left *m_time_pressed_left));
		if (abs(movement_force.x) < friction_scalar)
			movement_force.x = -friction_scalar;
		//if (m_vel.x > -MIN_VEL) m_vel.x = -MIN_VEL;
	}
	else m_time_pressed_left = 0;
	//RIGHT
	if (m_key_right)
	{
		m_time_pressed_right += ms;
		movement_force.x = -(BUTTON_FORCE - (m_time_pressed_right*m_time_pressed_right));
		if (abs(movement_force.x) < friction_scalar)
			movement_force.x = friction_scalar;
		//if (m_vel.x < MIN_VEL) m_vel.x = MIN_VEL;
	}
	else m_time_pressed_right = 0;
	if (!m_key_left && !m_key_right)
	{
		if ((m_vel.x > 0 && m_vel.x < 20.f) || m_vel.x < 0 && m_vel.x > -20.f) {
			m_vel.x = 0;
			m_accel.x = 0;
		}
	}


	//simulation steps
	//sum forces

	force_sum.x += movement_force.x;
	force_sum.y += movement_force.y;
  
  //set friction to oppose velocity
	if (m_vel.x > 0)
		friction.x = -friction_scalar;
	else if (m_vel.x < 0)
		friction.x = friction_scalar;
	if (m_vel.y > 0)
		friction.y = -friction_scalar;
	else if (m_vel.y < 0)
		friction.y = friction_scalar;

	force_sum.x += friction.x;
	force_sum.y += friction.y;

	m_accel.x += force_sum.x * time_factor;
	m_accel.y += force_sum.y * time_factor;
  
  //clamp acceleration
	if (m_accel.x > MAX_ACCEL) {
		m_accel.x = MAX_ACCEL;
	}
	else if (m_accel.x < -MAX_ACCEL) {
		m_accel.x = -MAX_ACCEL;
	}
	if (m_accel.y > MAX_ACCEL) {
		m_accel.y = MAX_ACCEL;
	}
	else if (m_accel.y < -MAX_ACCEL) {
		m_accel.y = -MAX_ACCEL;
  }
  
	m_vel.x += m_accel.x * time_factor;
	m_vel.y += m_accel.y * time_factor;
  
  //clamp velocity
	if (m_vel.x > MAX_VEL) {
		m_vel.x = MAX_VEL;
	}
	else if (m_vel.x < -MAX_VEL) {
		m_vel.x = -MAX_VEL;
	}
	if (m_vel.y > MAX_VEL) {
		m_vel.y = MAX_VEL;
	}
	else if (m_vel.y < -MAX_VEL) {
		m_vel.y = -MAX_VEL;
	}
  
  //Wall collision reactions (no change in force modelled yet)
	if (!(can_move_up)){
		if (m_vel.y < 0) {
			m_vel.y = 0;
		}
	}
	if (!(can_move_down)){
		if (m_vel.y > 0) {
			m_vel.y = 0;
		}
	}
	if (!(can_move_left)){
		if (m_vel.x < 0) {
			m_vel.x = 0;
		}
	}
	if (!(can_move_right)){
		if (m_vel.x > 0) {
			m_vel.x = 0;
		}
	}

	m_position.x = m_position.x + m_vel.x * time_factor;
	m_position.y = m_position.y + m_vel.y * time_factor;
  
	pick_movement_tex();
}

void Janitor::check_collisions()
{
  bool door_collision_this_frame = false;
  for (Room::adjacent_room& adjacent : m_dungeon->get_adjacent_rooms(m_currentRoom->getRoomID()))
  {
    if (collides_with(*adjacent.door, m_dungeon->transform, identity_matrix)) // door is in dungeon coords 
    {
      if (!m_door_collision_last_frame)
      {
        set_current_room(adjacent.room);
      }
      door_collision_this_frame = true;
      break; // don't need to check against other doors for this frame
    }
  }

  m_door_collision_last_frame = door_collision_this_frame;
}

void Janitor::update_children(float ms) {}

//Pass texcoord offsets to the shader
void Janitor::draw_current(const mat3& projection, const mat3& current_transform) 
{
	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	//animation uniforms
	GLint frame_uloc = glGetUniformLocation(effect.program, "frame");
	GLint animation_dir_uloc = glGetUniformLocation(effect.program, "animation_dir");
	GLfloat animation_frame_w_uloc = glGetUniformLocation(effect.program, "animation_frame_w");
	GLfloat animation_frame_h_uloc = glGetUniformLocation(effect.program, "animation_frame_h");


	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	

	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);

	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
	
	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, janitor_sheet.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	//animation uniforms
	glUniform1iv(frame_uloc, 1, &frame);
	glUniform1iv(animation_dir_uloc, 1, (int*)&animation_dir);
	glUniform1fv(animation_frame_w_uloc, 1, &animation_frame_w);
	glUniform1fv(animation_frame_h_uloc, 1, &animation_frame_h);
	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
void Janitor::draw_children(const mat3& projection, const mat3& current_transform) {

}

void Janitor::set_accel(vec2 newAccel) { m_accel = newAccel; }
void Janitor::set_vel(vec2 newVel) { m_vel = newVel; }

void Janitor::set_dungeon(Dungeon* dungeon) { m_dungeon = dungeon; }

void Janitor::set_current_room(Room* room)
{
	m_currentRoom = room;
}

Room* Janitor::get_current_room()
{
  return m_currentRoom;
}

int Janitor::get_current_room_id()
{
	return m_currentRoom->getRoomID();
}

void Janitor::check_movement()
{
	float jLeftEdge = m_position.x;
  float jRightEdge = m_position.x+m_size.x;
  float jTopEdge = m_position.y;
  float jBottomEdge = m_position.y+m_size.y;
  float wLeftEdge, wRightEdge, wTopEdge, wBottomEdge;
  float wLeftEdge2, wRightEdge2, wTopEdge2, wBottomEdge2;

  vec2 wall, wall2;

	can_move_up = true;
	can_move_down = true;
	can_move_left = true;
	can_move_right = true;

	if (m_currentRoom->getRoomID() == -1) {
  	for (std::unique_ptr<Room>& room : m_dungeon->get_rooms()) {
			std::vector<Wall>& walls = room->get_walls();
			for (Wall& w : walls) {
				wall = get_world_coords_from_room_coords(w.get_pos(), room->transform, m_dungeon->transform);
				wLeftEdge = wall.x;
				wRightEdge = wall.x + w.get_size().x;
				wTopEdge = wall.y;
				wBottomEdge = wall.y + w.get_size().y;

				if (jTopEdge - wBottomEdge <= 60.f && jTopEdge - wBottomEdge >= 0.f && abs (jLeftEdge-wLeftEdge) <= 45.f){
					can_move_up = false;
				}
				if (wTopEdge - jBottomEdge <= 60.f && wTopEdge - jBottomEdge >= 0.f && abs (jLeftEdge-wLeftEdge) <= 40.f){
					can_move_down = false;
				}
				if (jLeftEdge - wRightEdge <= 25.f && jLeftEdge - wRightEdge >= 0.f && abs (jTopEdge - wTopEdge) <= 35.f){
					can_move_left = false;
				}
				if (wLeftEdge - jRightEdge <= 35.f && wLeftEdge - jRightEdge >= 0.f && abs (jTopEdge - wTopEdge) <= 35.f){
					can_move_right = false;
				}
			}
		}
	} else {
		std::vector<Wall>& walls = m_currentRoom->get_walls();
		for (Wall& w : walls) {
			wall2 = get_world_coords_from_room_coords(w.get_pos(), m_currentRoom->transform, m_dungeon->transform);
			wLeftEdge2 = wall2.x;
			wRightEdge2 = wall2.x + w.get_size().x;
			wTopEdge2 = wall2.y;
			wBottomEdge2 = wall2.y + w.get_size().y;

			if (jTopEdge - wBottomEdge2 <= 60.f && jTopEdge - wBottomEdge2 >= 0.f && abs (jLeftEdge-wLeftEdge2) <= 45.f){
				can_move_up = false;
			}
			if (wTopEdge2 - jBottomEdge <= 60.f && wTopEdge2 - jBottomEdge >= 0.f && abs (jLeftEdge-wLeftEdge2) <= 40.f){
				can_move_down = false;
			}
			if (jLeftEdge - wRightEdge2 <= 25.f && jLeftEdge - wRightEdge2 >= 0.f && abs (jTopEdge - wTopEdge2) <= 35.f){
				can_move_left = false;
			}
			if (wLeftEdge2 - jRightEdge <= 35.f && wLeftEdge2 - jRightEdge >= 0.f && abs (jTopEdge - wTopEdge2) <= 35.f){
				can_move_right = false;
			}
		}
	}
}

void Janitor::key_up(bool move) {
	if (move) {
		m_key_up = true;
	} else {
		m_key_up = false;
	}
}

void Janitor::key_down(bool move) {
	if (move) {
		m_key_down = true;
	} else {
		m_key_down = false;
	}
}
void Janitor::key_left(bool move) {
	if (move) {
		m_key_left = true;
	} else {
		m_key_left = false;
	}
}
void Janitor::key_right(bool move) {
	if (move) {
		m_key_right = true;
	} else {
		m_key_right = false;
	}
}

//Helper function for draw_current
//Picks direction of animation based on m_vel
void Janitor::pick_movement_tex() {
	
	//if velocity is 0, return, leaving the current animation frame as is
	float x_vel = abs(m_vel.x);
	float y_vel = abs(m_vel.y);
	float vel_magnitude = y_vel;
	if (x_vel > y_vel)
		vel_magnitude = x_vel;
	if (vel_magnitude == 0) return;

	//Pick current texture based on direction of velocity
	vec2 vel_dir = normalize(m_vel);
	vec2 default_dir = { 1, 0 };
	float theta = acos(dot(vel_dir, default_dir)); //gives the angle of our velocity (but only from 0-pi in rads)
	if (vel_dir.y > 0) theta = -theta;	//flip negative values for the bottom half of the unit circle
	float pi = atan(1) * 4;
	animation_dir = right;
	if (theta < 3 * pi / 8 && theta > pi / 8)
		animation_dir = up_right;
	else if (theta < 5 * pi / 8 && theta > 3 * pi / 8)
		animation_dir = up;
	else if (theta < 7 * pi / 8 && theta > 5 * pi / 8)
		animation_dir = up_left;
	else if (theta < -5 * pi / 8 && theta > -7 * pi / 8)
		animation_dir = down_left;
	else if (theta < -3 * pi / 8 && theta > -5 * pi / 8)
		animation_dir = down;
	else if (theta < -pi / 8 && theta > -3 * pi / 8)
		animation_dir = down_right;
	//odd case
	else if (theta > 7 * pi / 8 || theta < -7 * pi / 8)
		animation_dir = left;
	//if (theta < pi / 8 || theta > -pi / 8)
	else
		animation_dir = right;
}
//Loads and validates texture files via hardcoded path names
bool Janitor::validate_textures()
{
	if (!janitor_sheet.is_valid())
	{
		if (!janitor_sheet.load_from_file(textures_path("janitor/janitorsheet.png")))
		{
			fprintf(stderr, "Failed to load janitor spritesheet\n");
			return false;
		}
	}
	return true;
}
