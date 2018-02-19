// hero.cpp

#include "hero.hpp"

Texture Hero::hero_texture;

Hero::Hero() {}

Hero::~Hero() {}



bool Hero::init()
{
	return init({ 0.f, 0.f });
}

bool Hero::init(vec2 position)
{
	if (!hero_texture.is_valid())
	{
		if (!hero_texture.load_from_file(textures_path("hero_placeholder.png")))
		{
			fprintf(stderr, "Failed to load hero texture\n");
			return false;
		}
	}

	m_position = position;
	m_vel = { 0.f, 0.f };
	reset_destination();

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
}

void Hero::setRoom(Room * room)
{
	m_currentRoom = room;
}

void Hero::set_destination(vec2 destination)
{
	m_destination = destination;
}

// call to stop hero movement
void Hero::reset_destination()
{
	m_destination = { -10.f, -10.f };
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
	// only move if a destination is set
	if (m_destination.x > 0 && m_destination.y > 0)
	{
		float step_size = 10.f; // should probably replace this with collisions 
		const float SPEED = 100.0f;
		//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
		const float Y_SPEED = SPEED * (24.f / 35.f);
		float timeFactor = ms / 1000;
		bool will_move = false;

		float s_x = m_position.x;
		float s_y = m_position.y;
		float d_x = m_destination.x;
		float d_y = m_destination.y;

		if (s_x - d_x > step_size)
		{
			will_move = true;
			m_vel.x = -SPEED;
		}
		else if (d_x - s_x > step_size)
		{
			will_move = true;
			m_vel.x = SPEED;
		}

		if (s_y - d_y > step_size)
		{
			will_move = true;
			m_vel.y = -Y_SPEED;
		}
		else if (d_y - s_y > step_size)
		{
			will_move = true;
			m_vel.y = Y_SPEED;
		}

		if (will_move) 
		{
			float new_position_x = m_position.x + m_vel.x * timeFactor;
			float new_position_y = m_position.y + m_vel.y * timeFactor;

			m_position.x = new_position_x;
			m_position.y = new_position_y;
		} 
		else
		{
			reset_destination();
		}
	}
	
}


void Hero::calculate_room_rewards()
{
	//Room * room;

	//room->setReward(calculate_best_neighbor_room(room)->getReward + PENALTY_VALUE);
}

Room * Hero::calculate_best_neighbor_room(Room * room)
{
	int test[3][2];
	test[0][1] = 1;
	return room;
}

