// hero.cpp

#include "hero.hpp"

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
		if (!hero_texture.load_from_file(textures_path("placeholders/hero_placeholder.png")))
		{
			fprintf(stderr, "Failed to load hero texture\n");
			return false;
		}
	}

	m_position = position;
	m_vel = { 0.f, 0.f };

	// The position corresponds to the center of the texture
	float wr = hero_texture.width * 0.5f;
	float hr = hero_texture.height * 0.5f;

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
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial scale values
	m_scale.x = 1.f;
	m_scale.y = 1.f;

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
	if (m_currentRoom->containsBoss())
	{
		stop_movement();
	}
	else if (!is_moving())
	{
		if (m_currentRoom->get_artifact()->is_activated())
		{
			vec2 artifact_pos = get_world_coords_from_room_coords(m_currentRoom->get_artifact()->get_pos(), m_currentRoom->transform, m_dungeon->transform);
			set_destination(artifact_pos, Hero::destinations::ARTIFACT);
			vector<vec2> path_to_artifact;
			Pathfinder::getPathFromPositionToDestination(m_position, artifact_pos, SPEED / 10.f, Y_SPEED / 10.f, path_to_artifact);
			m_path = path_to_artifact;
			m_current_destination = m_path.back();
			m_path.pop_back();
		}
		else
		{
      vec2 next_door_pos = get_next_door_position();
			set_destination(next_door_pos, Hero::destinations::DOOR);
			vector<vec2> path_to_door;
			Pathfinder::getPathFromPositionToDestination(m_position, next_door_pos, SPEED / 10.f, Y_SPEED / 10.f, path_to_door);
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

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}


void Hero::update_current(float ms)
{
	update_path();
	if (m_is_moving)
	{
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
			if (m_path.empty())
			{
				stop_movement();
				if (m_destination_type == DOOR)
				{
					m_currentRoom = m_next_room;
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
	}
	
}


vec2 Hero::get_next_door_position()
{
	float percentage_of_activated_artifacts = 0.0;
	int num_artifacts = 0;
	int num_activated_artifacts = 0;

	for (unique_ptr<Room>& room : *m_rooms)
	{
		if (room->containsUndiscoveredArtifact())
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
