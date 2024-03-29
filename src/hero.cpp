// hero.cpp

#include "hero.hpp"

#define ARTIFACT_OFFSET 60.f

Texture Hero::hero_texture;

Hero::Hero() {}

Hero::~Hero() {}

bool Hero::init()
{
	m_is_moving = false;
	m_next_room = nullptr;
	return init({ 0.f, 0.f });
}

bool Hero::init(vec2 position)
{
	if (!hero_texture.is_valid())
	{
		if (!hero_texture.load_from_file(textures_path("dungeon1/d1_hero_sheet.png")))
		{
			fprintf(stderr, "Failed to load hero texture\n");
			return false;
		}
	}

	m_position = position;
	m_is_at_boss = false;
	m_currentRoom->set_hero_has_visited(true);
	m_vel = { 0.f, 0.f };
	m_time_elapsed = 0;
	m_size = { hero_texture.width / 4.f , hero_texture.height / 4.f };

	animation_dir = right;
	frame = 0;
	// The position corresponds to the center of the texture
	float wr = hero_texture.width/4.f * 0.5f;
	float hr = hero_texture.height/4.f * 0.5f;
	animation_frame_w = 1 / 4.0f;
	animation_frame_h = 1 / 4.0f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, animation_frame_h };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { animation_frame_w, animation_frame_h };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { animation_frame_w, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f,  0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
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
	m_size = { static_cast<float>(hero_texture.width) / 4.f, static_cast<float>(hero_texture.height) / 4.f };
	m_scale = { 1.5f, 1.5f };


	// Setting initial size
	m_size = { static_cast<float>(hero_texture.width) / 4, static_cast<float>(hero_texture.height) / 4};

	return true;
}

void Hero::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);
	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Hero::setRoom(Room * room)
{
	m_currentRoom = room;
}

void Hero::setDungeon(Dungeon* dungeon)
{
	m_dungeon = dungeon;
}

void Hero::setAllRooms(vector<unique_ptr<Room>>* rooms)
{
	m_rooms = rooms;
}

void Hero::set_destination(vec2 destination_pos, Hero::destinations destination_type )
{
	m_is_moving = true;
	m_end_destination = destination_pos;
	m_destination_type = destination_type;
}

void Hero::stop_movement()
{
	m_is_moving = false;
}

void Hero::update_path()
{
	if (!is_moving() && !m_is_at_boss)
	{
		if (m_currentRoom->containsBoss())
		{
			vec2 boss_pos = m_dungeon->get_boss()->get_pos();
			set_destination(boss_pos, Hero::destinations::BOSS);
			vector<vec2> path_to_boss;
			Pathfinder::getPathFromPositionToDestination(m_position, boss_pos, SPEED / 10.f, Y_SPEED / 10.f,
				*this, *m_currentRoom, path_to_boss, *m_dungeon);
			m_path = path_to_boss;
			m_current_destination = m_path.back();
			m_path.pop_back();
		}
		else if (m_currentRoom->get_artifact()->is_activated())
		{
			vec2 artifact_pos = get_world_coords_from_room_coords(m_currentRoom->get_artifact()->get_pos(), m_currentRoom->transform, m_dungeon->transform);
			artifact_pos.y += ARTIFACT_OFFSET;
			set_destination(artifact_pos, Hero::destinations::ARTIFACT);
			vector<vec2> path_to_artifact;
			Pathfinder::getPathFromPositionToDestination(m_position, artifact_pos, SPEED / 10.f, Y_SPEED / 10.f,
				*this, *m_currentRoom, path_to_artifact, *m_dungeon);

			m_path = path_to_artifact;
			m_current_destination = m_path.back();
			m_path.pop_back();
		}
		else
		{
			vec2 next_door_pos = get_next_door_position();
			set_destination(next_door_pos, Hero::destinations::DOOR);
			vector<vec2> path_to_door;
			Pathfinder::getPathFromPositionToDestination(m_position, next_door_pos, SPEED / 10.f, Y_SPEED / 10.f,
				*this, *m_currentRoom, path_to_door, *m_dungeon);

			m_path = path_to_door;
			m_current_destination = m_path.back();
			m_path.pop_back();
		}
	}
}
bool Hero::is_moving()
{
	return m_is_moving;
}

bool Hero::is_at_boss()
{
	return m_is_at_boss;
}

const Room* Hero::get_current_room()
{
	return m_currentRoom;
}

void Hero::draw_current(const mat3& projection, const mat3& current_transform)
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
	glBindTexture(GL_TEXTURE_2D, hero_texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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


void Hero::update_current(float ms)
{
	update_path();
	if (m_is_moving)
	{
		m_vel = { 0,0 };
		m_time_elapsed += ms;
		if (m_time_elapsed > MS_PER_FRAME) {
			m_time_elapsed = 0;
			frame = (frame + 1) % NUM_FRAMES;
		}
		float step_size = 10.f; // should probably replace this with collisions 
		float timeFactor = ms / 1000;
		bool will_move_x = false;
		bool will_move_y = false;

		float s_x = m_position.x;
		float s_y = m_position.y;
		float d_x = m_current_destination.x;
		float d_y = m_current_destination.y;

		if (s_x - d_x > step_size)
		{
			will_move_x = true;
			m_vel.x = -SPEED;
		}
		else if (d_x - s_x > step_size)
		{
			will_move_x = true;
			m_vel.x = SPEED;
		}

		if (s_y - d_y > step_size)
		{
			will_move_y = true;
			m_vel.y = -Y_SPEED;
		}
		else if (d_y - s_y > step_size)
		{
			will_move_y = true;
			m_vel.y = Y_SPEED;
		}

		if (will_move_x || will_move_y) 
		{
			if (will_move_x)
			{
				float new_position_x = m_position.x + m_vel.x * timeFactor;
				m_position.x = new_position_x;
			}
			if (will_move_y)
			{
				float new_position_y = m_position.y + m_vel.y * timeFactor;
				m_position.y = new_position_y;
			}
		} 
		else
		{

			if (m_destination_type == BOSS && this->collides_with(*m_dungeon->get_boss(), identity_matrix, identity_matrix))
			{
				//m_is_at_boss = true;
				stop_movement();
				m_dungeon->start_boss_fight();
			}
			else if (m_path.empty())
			{
				stop_movement();
				if (m_destination_type == DOOR)
				{
					m_currentRoom = m_next_room;
					m_currentRoom->set_hero_has_visited(true);
				}
				else if (m_destination_type == ARTIFACT)
				{
					m_currentRoom->deactivate_artifact();
				}
			}
			else
			{
				m_current_destination = m_path.back();
				m_path.pop_back();
			}
		}
		pick_movement_tex();
	}
	
}


vec2 Hero::get_next_door_position()
{
	float percentage_of_activated_artifacts = 0.0;
	int num_artifacts = 0;
	int num_activated_artifacts = 0;

	for (unique_ptr<Room>& room : *m_rooms)
	{
		if (room->containsArtifact())
		{
			num_artifacts++;
			if (room->get_artifact()->is_activated()) {
				num_activated_artifacts++;
			}
		}

	}
	if (num_artifacts > 0)
	{
		percentage_of_activated_artifacts = (float)num_activated_artifacts / (float)num_artifacts;
	}


	Room::adjacent_room target_room = ValueIteration::getNextRoom(m_currentRoom, *m_rooms, percentage_of_activated_artifacts, *m_dungeon);

  m_next_room = target_room.room;
  return get_world_coords_from_room_coords(target_room.door->get_pos(), m_dungeon->transform, identity_matrix); // door is in dungeon coords
}
//RIPped it right of janitor.
void Hero::pick_movement_tex() {
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
	if (theta < 3 * pi / 4 && theta > pi / 4) {
		animation_dir = up;
	}
	else if (theta < -pi / 4 && theta > -3 * pi / 4) {
		animation_dir = down;
	}
	//odd case
	else if (theta > 3 * pi / 4 || theta < -3*pi / 4) {
		animation_dir = left;
	}
	//if (theta < pi / 4 || theta > -pi / 4)
	else {
		animation_dir = right;
	}
}