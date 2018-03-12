#include "janitor.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include "wall.hpp"

//This is ugly af
Texture Janitor::up1; Texture Janitor::up2; Texture Janitor::up3; Texture Janitor::up4;
Texture Janitor::up_right1; Texture Janitor::up_right2; Texture Janitor::up_right3; Texture Janitor::up_right4;
Texture Janitor::up_left1; Texture Janitor::up_left2; Texture Janitor::up_left3; Texture Janitor::up_left4;
Texture Janitor::down1; Texture Janitor::down2; Texture Janitor::down3; Texture Janitor::down4;
Texture Janitor::down_right1; Texture Janitor::down_right2; Texture Janitor::down_right3; Texture Janitor::down_right4;
Texture Janitor::down_left1; Texture Janitor::down_left2; Texture Janitor::down_left3; Texture Janitor::down_left4;
Texture Janitor::left1; Texture Janitor::left2; Texture Janitor::left3; Texture Janitor::left4;
Texture Janitor::right1; Texture Janitor::right2; Texture Janitor::right3; Texture Janitor::right4;

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
	//all the player textures are the same size so it's fine to pick one arbitrarily
	float wr = up2.width * 0.5f;
	float hr = up2.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f, 0.f };

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
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	//Initialize member variables
	m_position = position;
	m_size = {static_cast<float>(up2.width), static_cast<float>(up2.height)};
	m_animation_time = 0;
	m_time_pressed_up = 0;
	m_time_pressed_down = 0;
	m_time_pressed_left = 0;
	m_time_pressed_right = 0;
	m_key_up = false;
	m_key_down = false;
	m_key_left = false;
	m_key_right = false;
	m_curr_tex = &up1;
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
	float const BUTTON_FORCE = 360.f;
	//Friction constant of concrete
	float const FRICTIONAL_CONST = 0.6;
	float const MASS = 60.f;
	float const G = 9.81;
	float friction_scalar = FRICTIONAL_CONST * MASS * G;
	friction_scalar = friction_scalar * friction_scalar;
	/* MIN_VEL is holdover, currently it has no effect, but it may be useful later, please leave for now*/
	float const MIN_VEL = 0.f;
	float const MAX_VEL = 220.f;
	float const MAX_ACCEL = 600.f;

	//const float SPEED = 180.0f;
	//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
	//const float Y_SPEED = SPEED * (24.f / 35.f);
	float time_factor = ms / 1000;

	const int NUM_FRAMES = 4;	//4 frames of animation per direction
	const int FRAME_TIMING = 80; //80 ms per frame of animation (12.5fps)

	if (m_animation_time > FRAME_TIMING * NUM_FRAMES)
		m_animation_time = 0;
	else
		m_animation_time += ms;
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

	m_position.x = m_position.x + m_vel.x * time_factor;
	m_position.y = m_position.y + m_vel.y * time_factor;
	//Check collisions here.

	//Pick current texture based on direction of velocity
	vec2 vel_dir = normalize(m_vel);
	vec2 default_dir = { 1, 0 };
	float theta = acos(dot(vel_dir, default_dir)); //gives the angle of our velocity (but only from 0-pi in rads)
	if (vel_dir.y > 0) theta = -theta;	//flip negative values for the bottom half of the unit circle
	float pi = atan(1) * 4;
	DIRECTION animation_dir = right;
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
	pick_movement_tex(animation_dir, FRAME_TIMING);

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
	//dereferencing a null pointer ic ic.
	glBindTexture(GL_TEXTURE_2D, m_curr_tex->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
void Janitor::draw_children(const mat3& projection, const mat3& current_transform) {

}

void Janitor::set_accel(vec2 newAccel) { m_accel = newAccel; }
void Janitor::set_vel(vec2 newVel) { m_vel = newVel; }

void Janitor::set_room(int id)
{
	m_currentRoom = id;
}

int Janitor::get_current_room_id()
{
	return m_currentRoom;
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
//sets *m_curr_tex = &some_tex where some_tex is based on timing and key(s) pressed
void Janitor::pick_movement_tex(DIRECTION dir, const int FRAME_TIMING) {
	float x_vel = abs(m_vel.x);
	float y_vel = abs(m_vel.y);
	float vel_magnitude = y_vel;
	if (x_vel > y_vel)
		vel_magnitude = x_vel;
	if (vel_magnitude == 0) return;
	switch (dir)
	{
		case up:
		{
			if (m_animation_time < FRAME_TIMING)	
				m_curr_tex = &up1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)	
				m_curr_tex = &up2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)	
				m_curr_tex = &up3;
			else if (m_animation_time >= 3 * FRAME_TIMING)
				m_curr_tex = &up4;
			break;
		}
		case up_right:
		{
			if (m_animation_time < FRAME_TIMING)						
				m_curr_tex = &up_right1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)	
				m_curr_tex = &up_right2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)	
				m_curr_tex = &up_right3;
			else if (m_animation_time >= 3 * FRAME_TIMING)				
				m_curr_tex = &up_right4;
			break;
		}
		case up_left:
		{
			if (m_animation_time < FRAME_TIMING)
				m_curr_tex = &up_left1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)
				m_curr_tex = &up_left2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)
				m_curr_tex = &up_left3;
			else if (m_animation_time >= 3 * FRAME_TIMING)	
				m_curr_tex = &up_left4;
			break;
		}
		case down:
		{
			if (m_animation_time < FRAME_TIMING)
				m_curr_tex = &down1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)
				m_curr_tex = &down2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)
				m_curr_tex = &down3;
			else if (m_animation_time >= 3 * FRAME_TIMING)
				m_curr_tex = &down4;
			break;
		}
		case down_right:
		{
			if (m_animation_time < FRAME_TIMING)	
				m_curr_tex = &down_right1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)	
				m_curr_tex = &down_right2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)	
				m_curr_tex = &down_right3;
			else if (m_animation_time >= 3 * FRAME_TIMING)
				m_curr_tex = &down_right4;
			break;
		}
		case down_left:
		{
			if (m_animation_time < FRAME_TIMING)	
				m_curr_tex = &down_left1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)	
				m_curr_tex = &down_left2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)	
				m_curr_tex = &down_left3;
			else if (m_animation_time >= 3 * FRAME_TIMING)				
				m_curr_tex = &down_left4;
			break;
		}
		case right:
		{
			if (m_animation_time < FRAME_TIMING)	
				m_curr_tex = &right1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)	
				m_curr_tex = &right2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)	
				m_curr_tex = &right3;
			else if (m_animation_time >= 3 * FRAME_TIMING)
				m_curr_tex = &right4;
			break;
		}
		case left:
		{
			if (m_animation_time < FRAME_TIMING)	
				m_curr_tex = &left1;
			else if (m_animation_time >= FRAME_TIMING && m_animation_time < 2 * FRAME_TIMING)	
				m_curr_tex = &left2;
			else if (m_animation_time >= 2 * FRAME_TIMING && m_animation_time < 3 * FRAME_TIMING)	
				m_curr_tex = &left3;
			else if (m_animation_time >= 3 * FRAME_TIMING) 
				m_curr_tex = &left4;
		}
	}
}
//Loads and validates texture files via hardcoded path names
bool Janitor::validate_textures()
{
	if (!up1.is_valid())
	{
		if (!up1.load_from_file(textures_path("janitor/up1.png")))
		{
			fprintf(stderr, "Failed to load texture up1\n");
			return false;
		}
	}
	if (!up2.is_valid())
	{
		if (!up2.load_from_file(textures_path("janitor/up2.png")))
		{
			fprintf(stderr, "Failed to load texture up2\n");
			return false;
		}
	}
	if (!up3.is_valid())
	{
		if (!up3.load_from_file(textures_path("janitor/up3.png")))
		{
			fprintf(stderr, "Failed to load texture up3\n");
			return false;
		}
	}
	if (!up4.is_valid())
	{
		if (!up4.load_from_file(textures_path("janitor/up4.png")))
		{
			fprintf(stderr, "Failed to load texture up4\n");
			return false;
		}
	}
	if (!down1.is_valid())
	{
		if (!down1.load_from_file(textures_path("janitor/down1.png")))
		{
			fprintf(stderr, "Failed to load texture down1\n");
			return false;
		}
	}
	if (!down2.is_valid())
	{
		if (!down2.load_from_file(textures_path("janitor/down2.png")))
		{
			fprintf(stderr, "Failed to load texture down2\n");
			return false;
		}
	}
	if (!down3.is_valid())
	{
		if (!down3.load_from_file(textures_path("janitor/down3.png")))
		{
			fprintf(stderr, "Failed to load texture down3\n");
			return false;
		}
	}
	if (!down4.is_valid())
	{
		if (!down4.load_from_file(textures_path("janitor/down4.png")))
		{
			fprintf(stderr, "Failed to load texture down4\n");
			return false;
		}
	}
	if (!left1.is_valid())
	{
		if (!left1.load_from_file(textures_path("janitor/left1.png")))
		{
			fprintf(stderr, "Failed to load texture left1\n");
			return false;
		}
	}
	if (!left2.is_valid())
	{
		if (!left2.load_from_file(textures_path("janitor/left2.png")))
		{
			fprintf(stderr, "Failed to load texture left2\n");
			return false;
		}
	}
	if (!left3.is_valid())
	{
		if (!left3.load_from_file(textures_path("janitor/left3.png")))
		{
			fprintf(stderr, "Failed to load texture left3\n");
			return false;
		}
	}
	if (!left4.is_valid())
	{
		if (!left4.load_from_file(textures_path("janitor/left4.png")))
		{
			fprintf(stderr, "Failed to load texture left4\n");
			return false;
		}
	}
	if (!right1.is_valid())
	{
		if (!right1.load_from_file(textures_path("janitor/right1.png")))
		{
			fprintf(stderr, "Failed to load texture right1\n");
			return false;
		}
	}
	if (!right2.is_valid())
	{
		if (!right2.load_from_file(textures_path("janitor/right2.png")))
		{
			fprintf(stderr, "Failed to load texture right2\n");
			return false;
		}
	}
	if (!right3.is_valid())
	{
		if (!right3.load_from_file(textures_path("janitor/right3.png")))
		{
			fprintf(stderr, "Failed to load texture right3\n");
			return false;
		}
	}
	if (!right4.is_valid())
	{
		if (!right4.load_from_file(textures_path("janitor/right4.png")))
		{
			fprintf(stderr, "Failed to load texture right4\n");
			return false;
		}
	}
	if (!up_right1.is_valid())
	{
		if (!up_right1.load_from_file(textures_path("janitor/up_right1.png")))
		{
			fprintf(stderr, "Failed to load texture up_right1\n");
			return false;
		}
	}
	if (!up_right2.is_valid())
	{
		if (!up_right2.load_from_file(textures_path("janitor/up_right2.png")))
		{
			fprintf(stderr, "Failed to load texture up_right2\n");
			return false;
		}
	}
	if (!up_right3.is_valid())
	{
		if (!up_right3.load_from_file(textures_path("janitor/up_right3.png")))
		{
			fprintf(stderr, "Failed to load texture up_right3\n");
			return false;
		}
	}
	if (!up_right4.is_valid())
	{
		if (!up_right4.load_from_file(textures_path("janitor/up_right4.png")))
		{
			fprintf(stderr, "Failed to load texture up_right4\n");
			return false;
		}
	}
	if (!up_left1.is_valid())
	{
		if (!up_left1.load_from_file(textures_path("janitor/up_left1.png")))
		{
			fprintf(stderr, "Failed to load texture up_left1\n");
			return false;
		}
	}
	if (!up_left2.is_valid())
	{
		if (!up_left2.load_from_file(textures_path("janitor/up_left2.png")))
		{
			fprintf(stderr, "Failed to load texture up_left2\n");
			return false;
		}
	}
	if (!up_left3.is_valid())
	{
		if (!up_left3.load_from_file(textures_path("janitor/up_left3.png")))
		{
			fprintf(stderr, "Failed to load texture up_left3\n");
			return false;
		}
	}
	if (!up_left4.is_valid())
	{
		if (!up_left4.load_from_file(textures_path("janitor/up_left4.png")))
		{
			fprintf(stderr, "Failed to load texture up_left4\n");
			return false;
		}
	}
	if (!down_right1.is_valid())
	{
		if (!down_right1.load_from_file(textures_path("janitor/down_right1.png")))
		{
			fprintf(stderr, "Failed to load texture down_right1\n");
			return false;
		}
	}
	if (!down_right2.is_valid())
	{
		if (!down_right2.load_from_file(textures_path("janitor/down_right2.png")))
		{
			fprintf(stderr, "Failed to load texture down_right2\n");
			return false;
		}
	}
	if (!down_right3.is_valid())
	{
		if (!down_right3.load_from_file(textures_path("janitor/down_right3.png")))
		{
			fprintf(stderr, "Failed to load texture down_right3\n");
			return false;
		}
	}
	if (!down_right4.is_valid())
	{
		if (!down_right4.load_from_file(textures_path("janitor/down_right4.png")))
		{
			fprintf(stderr, "Failed to load texture down_right4\n");
			return false;
		}
	}
	if (!down_left1.is_valid())
	{
		if (!down_left1.load_from_file(textures_path("janitor/down_left1.png")))
		{
			fprintf(stderr, "Failed to load texture down_left1\n");
			return false;
		}
	}
	if (!down_left2.is_valid())
	{
		if (!down_left2.load_from_file(textures_path("janitor/down_left2.png")))
		{
			fprintf(stderr, "Failed to load texture down_left2\n");
			return false;
		}
	}
	if (!down_left3.is_valid())
	{
		if (!down_left3.load_from_file(textures_path("janitor/down_left3.png")))
		{
			fprintf(stderr, "Failed to load texture down_left3\n");
			return false;
		}
	}
	if (!down_left4.is_valid())
	{
		if (!down_left4.load_from_file(textures_path("janitor/down_left4.png")))
		{
			fprintf(stderr, "Failed to load texture down_left4\n");
			return false;
		}
	}
	return true;
}
