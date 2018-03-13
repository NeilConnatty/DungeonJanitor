#include "janitor.hpp"
#include "wall.hpp"
#include "dungeon.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

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
	m_time_pressed = 0;
	m_key_up = false;
	m_key_down = false;
	m_key_left = false;
	m_key_right = false;
  m_door_collision_last_frame = false;
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
	const float SPEED = 180.0f;
	//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
	const float Y_SPEED = SPEED * (24.f / 35.f);
	float time_factor = ms / 1000;
	//ms is how long it's been since the previous iteration, ranges from 10 - 30, generally.
	if (!m_key_up && !m_key_down && !m_key_left && !m_key_right)
		m_time_pressed = 0;
	const int NUM_FRAMES = 4;	//4 frames of animation per direction
	const int FRAME_TIMING = 80; //80 ms per frame of animation (12.5fps)

  check_collisions();

	//UP
	if (m_key_up)
	{ 
		m_vel.y = -Y_SPEED;
		
		if (m_key_right)
		{
			m_vel.x = SPEED;
			pick_movement_tex(up_right, FRAME_TIMING);

		}
		else if (m_key_left)
		{
			m_vel.x = -SPEED;
			pick_movement_tex(up_left, FRAME_TIMING);
		}
		else 
		{
			pick_movement_tex(up, FRAME_TIMING);
		}		
	}
	//DOWN
	else if (m_key_down)
	{
		m_vel.y = Y_SPEED;
		if (m_key_right)
		{
			m_vel.x = SPEED;
			pick_movement_tex(down_right, FRAME_TIMING);
		}
		else if (m_key_left)
		{
			m_vel.x = -SPEED;
			pick_movement_tex(down_left, FRAME_TIMING);
		}
		else
		{
			pick_movement_tex(down, FRAME_TIMING);
		}
	}
	else
	{
		m_vel.y = 0;
	}
	//LEFT
	if (m_key_left)
	{
		m_vel.x = -SPEED;
		if (m_key_up)
		{
			m_vel.y = -Y_SPEED;
			pick_movement_tex(up_left, FRAME_TIMING);
		}
		else if (m_key_down)
		{
			m_vel.y = Y_SPEED;
			pick_movement_tex(down_left, FRAME_TIMING);
		}
		else
		{
			pick_movement_tex(left, FRAME_TIMING);
		}
	}
	//RIGHT
	else if (m_key_right) 
	{
		m_vel.x = SPEED;
		
		if (m_key_up)
		{
			m_vel.y = -Y_SPEED;
			pick_movement_tex(up_right, FRAME_TIMING);
		}
		else if (m_key_down)
		{
			m_vel.y = Y_SPEED;
			pick_movement_tex(down_right, FRAME_TIMING);
		}
		else 
		{
			pick_movement_tex(right, FRAME_TIMING);
		}
	}
	else
	{
		m_vel.x = 0;
	}

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

	if (m_time_pressed > FRAME_TIMING*NUM_FRAMES)
		m_time_pressed = 0;
	else m_time_pressed += ms;

	//std::cout << m_time_pressed;
	float new_position_x = m_position.x + m_vel.x * time_factor;
	float new_position_y = m_position.y + m_vel.y * time_factor;

	m_position.x = new_position_x;
	m_position.y = new_position_y;
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
//sets *m_curr_tex = &some_tex where some_tex is based on timing and key(s) pressed
void Janitor::pick_movement_tex(DIRECTION dir, const int FRAME_TIMING) {
	switch (dir)
	{
		case up:
		{
			if (m_time_pressed < FRAME_TIMING)	
				m_curr_tex = &up1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)	
				m_curr_tex = &up2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)	
				m_curr_tex = &up3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)
				m_curr_tex = &up4;
			break;
		}
		case up_right:
		{
			if (m_time_pressed < FRAME_TIMING)						
				m_curr_tex = &up_right1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)	
				m_curr_tex = &up_right2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)	
				m_curr_tex = &up_right3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)				
				m_curr_tex = &up_right4;
			break;
		}
		case up_left:
		{
			if (m_time_pressed < FRAME_TIMING)
				m_curr_tex = &up_left1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)
				m_curr_tex = &up_left2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)
				m_curr_tex = &up_left3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)	
				m_curr_tex = &up_left4;
			break;
		}
		case down:
		{
			if (m_time_pressed < FRAME_TIMING)
				m_curr_tex = &down1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)
				m_curr_tex = &down2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)
				m_curr_tex = &down3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)
				m_curr_tex = &down4;
			break;
		}
		case down_right:
		{
			if (m_time_pressed < FRAME_TIMING)	
				m_curr_tex = &down_right1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)	
				m_curr_tex = &down_right2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)	
				m_curr_tex = &down_right3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)
				m_curr_tex = &down_right4;
			break;
		}
		case down_left:
		{
			if (m_time_pressed < FRAME_TIMING)	
				m_curr_tex = &down_left1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)	
				m_curr_tex = &down_left2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)	
				m_curr_tex = &down_left3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)				
				m_curr_tex = &down_left4;
			break;
		}
		case right:
		{
			if (m_time_pressed < FRAME_TIMING)	
				m_curr_tex = &right1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)	
				m_curr_tex = &right2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)	
				m_curr_tex = &right3;
			else if (m_time_pressed >= 3 * FRAME_TIMING)
				m_curr_tex = &right4;
			break;
		}
		case left:
		{
			if (m_time_pressed < FRAME_TIMING)	
				m_curr_tex = &left1;
			else if (m_time_pressed >= FRAME_TIMING && m_time_pressed < 2 * FRAME_TIMING)	
				m_curr_tex = &left2;
			else if (m_time_pressed >= 2 * FRAME_TIMING && m_time_pressed < 3 * FRAME_TIMING)	
				m_curr_tex = &left3;
			else if (m_time_pressed >= 3 * FRAME_TIMING) 
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
