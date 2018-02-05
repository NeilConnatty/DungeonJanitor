#pragma once

#include "Janitor.hpp"

Texture placeholder_texture;

Janitor::Janitor() : GameObject()
{
}

Janitor::~Janitor() {}

//Init graphics resources
bool Janitor::init() { return false; }
bool Janitor::init(vec2 position) 
{

	if (!placeholder_texture.is_valid())
	{
		if (!placeholder_texture.load_from_file(textures_path("wall.png")))
		{
			fprintf(stderr, "Failed to load wall texture\n");
			return false;
		}
	}

	float wr = placeholder_texture.width * 0.5f;
	float hr = placeholder_texture.height * 0.5f;

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

	m_key_up = false;
	m_key_down = false;
	m_key_left = false;
	m_key_right = false;

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
	//hard code errthang :/
	const float MAX_ACCEL = 3.0f;
	float delta_accel = 0.8f;
	float delta_decel = 1.0f;
	//UP
	if (m_key_up && m_accel.y > -MAX_ACCEL) 
	{ 
		m_accel.y -= delta_accel; 
		if (m_accel.y <= -MAX_ACCEL) m_accel.y = -MAX_ACCEL;
	}
	else m_accel.y += delta_decel;
	//DOWN
	if (m_key_down && m_accel.y < MAX_ACCEL)
	{
		m_accel.y += delta_accel;
		if (m_accel.y > MAX_ACCEL) m_accel.y = MAX_ACCEL;
	}
	else m_accel.y -= delta_decel;
	//LEFT
	if (m_key_left && m_accel.x > -MAX_ACCEL) 
	{
		m_accel.x -= delta_accel;
		if (m_accel.x < -MAX_ACCEL) m_accel.x = -MAX_ACCEL;
	}
	else m_accel.x += delta_decel;
	//RIGHT
	if (m_key_right && m_accel.x > MAX_ACCEL) 
	{
		m_accel.x += delta_accel;
		if (m_accel.x > MAX_ACCEL) m_accel.x = MAX_ACCEL;
	}
	else m_accel.x -= delta_decel;


	m_vel.x += m_accel.x;
	m_vel.y += m_accel.y;

	m_position.x += m_vel.x;
	m_position.y += m_vel.y;

}
void Janitor::update_children(float ms) {}

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
	glBindTexture(GL_TEXTURE_2D, placeholder_texture.id);

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

void Janitor::key_up() { m_key_up = !m_key_up; }
void Janitor::key_down() { m_key_down = !m_key_down; }
void Janitor::key_left() { m_key_left = !m_key_left; }
void Janitor::key_right() { m_key_right = !m_key_right; }