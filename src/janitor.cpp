#include "janitor.hpp"
#include <algorithm>
#include <iostream>
//This is ugly.
//Texture Janitor::up1;
Texture Janitor::up2; Texture Janitor::up3; Texture Janitor::up4;
Texture Janitor::down1; Texture Janitor::down2; Texture Janitor::down3; Texture Janitor::down4;
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
	
	if (!up2.is_valid())
	{
		if (!up2.load_from_file(textures_path("up2.png")))
		{
			fprintf(stderr, "Failed to load texture up2\n");
			return false;
		}
	}
	if (!up3.is_valid())
	{
		if (!up3.load_from_file(textures_path("up3.png")))
		{
			fprintf(stderr, "Failed to load texture up3\n");
			return false;
		}
	}
	if (!up4.is_valid())
	{
		if (!up4.load_from_file(textures_path("up4.png")))
		{
			fprintf(stderr, "Failed to load texture up4\n");
			return false;
		}
	}
	if (!down1.is_valid())
	{
		if (!down1.load_from_file(textures_path("down1.png")))
		{
			fprintf(stderr, "Failed to load texture down1\n");
			return false;
		}
	}
	if (!down2.is_valid())
	{
		if (!down2.load_from_file(textures_path("down2.png")))
		{
			fprintf(stderr, "Failed to load texture down2\n");
			return false;
		}
	}
	if (!down3.is_valid())
	{
		if (!down3.load_from_file(textures_path("down3.png")))
		{
			fprintf(stderr, "Failed to load texture down3\n");
			return false;
		}
	}
	if (!down4.is_valid())
	{
		if (!down4.load_from_file(textures_path("down4.png")))
		{
			fprintf(stderr, "Failed to load texture down4\n");
			return false;
		}
	}
	if (!left1.is_valid())
	{
		if (!left1.load_from_file(textures_path("left1.png")))
		{
			fprintf(stderr, "Failed to load texture left1\n");
			return false;
		}
	}
	if (!left2.is_valid())
	{
		if (!left2.load_from_file(textures_path("left2.png")))
		{
			fprintf(stderr, "Failed to load texture left2\n");
			return false;
		}
	}
	if (!left3.is_valid())
	{
		if (!left3.load_from_file(textures_path("left3.png")))
		{
			fprintf(stderr, "Failed to load texture left3\n");
			return false;
		}
	}
	if (!left4.is_valid())
	{
		if (!left4.load_from_file(textures_path("left4.png")))
		{
			fprintf(stderr, "Failed to load texture left4\n");
			return false;
		}
	}
	if (!right1.is_valid())
	{
		if (!right1.load_from_file(textures_path("right1.png")))
		{
			fprintf(stderr, "Failed to load texture right1\n");
			return false;
		}
	}
	if (!right2.is_valid())
	{
		if (!right2.load_from_file(textures_path("right2.png")))
		{
			fprintf(stderr, "Failed to load texture right2\n");
			return false;
		}
	}
	if (!right3.is_valid())
	{
		if (!right3.load_from_file(textures_path("right3.png")))
		{
			fprintf(stderr, "Failed to load texture right3\n");
			return false;
		}
	}
	if (!right4.is_valid())
	{
		if (!right4.load_from_file(textures_path("right4.png")))
		{
			fprintf(stderr, "Failed to load texture right4\n");
			return false;
		}
	}

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
	m_key_cycles = 0;
	m_key_up = false;
	m_key_down = false;
	m_key_left = false;
	m_key_right = false;
	m_tex_sheet = &up2;
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
	const float SPEED = 200.0f;
	//Floor tiles are 35x24, this is the proportion for speed to be consistent depthwise.
	const float Y_SPEED = SPEED * (24.f / 35.f);
	float timeFactor = ms / 1000;
	
	//m_key_cycles represents how many consecutive times update has been called with a key pressed
	//Therefore it resets whenever there is no key pressed
	if (!m_key_up && !m_key_down && !m_key_left && !m_key_right)
		m_key_cycles = 0;
	//m_tex_index can take any value less than MOD, and the cases for each 
	//frame of animation are divided evenly as MOD/number of frames
	//Higher values of MOD (and associated constants in the conditionals)
	//results in slower animation cycles.
	const int MOD = 16;
	m_tex_index = m_key_cycles % MOD;

	//UP
	if (m_key_up)
	{ 
		m_vel.y = -Y_SPEED;
		
		if (m_tex_index <= 3)				m_tex_sheet = &up2;
		if (m_tex_index >= 5 && m_tex_index <= 8)	m_tex_sheet = &up2;
		if (m_tex_index >= 9 && m_tex_index <= 12)	m_tex_sheet = &up3;
		if (m_tex_index >= 13)				m_tex_sheet = &up4;
	}
	
	//DOWN
	else if (m_key_down)
	{
		m_vel.y = Y_SPEED;
		if (m_tex_index <= 3)				m_tex_sheet = &down1;
		if (m_tex_index >= 5 && m_tex_index <= 8)	m_tex_sheet = &down2;
		if (m_tex_index >= 9 && m_tex_index <= 12)	m_tex_sheet = &down3;
		if (m_tex_index >= 13)				m_tex_sheet = &down4;
	}
	
	else
	{
		m_vel.y = 0;
	}
	//LEFT
	if (m_key_left)
	{
		m_vel.x = -SPEED;
		if (m_tex_index <= 3)				m_tex_sheet = &left1;
		if (m_tex_index >= 5 && m_tex_index <= 8)	m_tex_sheet = &left2;
		if (m_tex_index >= 9 && m_tex_index <= 12)	m_tex_sheet = &left3;
		if (m_tex_index >= 13)				m_tex_sheet = &left4;
	}
	//RIGHT
	else if (m_key_right) 
	{
		m_vel.x = SPEED;
		if (m_tex_index <= 3)				m_tex_sheet = &right1;
		if (m_tex_index >= 5 && m_tex_index <= 8)	m_tex_sheet = &right2;
		if (m_tex_index >= 9 && m_tex_index <= 12)	m_tex_sheet = &right3;
		if (m_tex_index >= 13)				m_tex_sheet = &right4;

	}
	else
	{
		m_vel.x = 0;
	}
	m_key_cycles++;
	//std::cout << m_tex_index;
	float new_position_x = m_position.x + m_vel.x * timeFactor;
	float new_position_y = m_position.y + m_vel.y * timeFactor;

	m_position.x = new_position_x;
	m_position.y = new_position_y;

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
	glBindTexture(GL_TEXTURE_2D, m_tex_sheet->id);

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
