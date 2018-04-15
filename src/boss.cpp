// boss.cpp

#include "boss.hpp"

Texture Boss::boss_texture;

Boss::Boss() {}

Boss::~Boss() {}

bool Boss::init()
{
	return init({ 0.f, 0.f });
}

bool Boss::init(vec2 position)
{
	if (!boss_texture.is_valid())
	{
		if (!boss_texture.load_from_file(textures_path("dungeon1/d1_boss_sheet.png")))
		{
			fprintf(stderr, "Failed to load boss texture\n");
			return false;
		}
	}

	m_position = position;
	m_size = { boss_texture.width / 4.f * 0.5f, boss_texture.height / 2.f * 0.5f };

	// The position corresponds to the center of the texture
	float wr = boss_texture.width/4.f * 0.5f;
	float hr = boss_texture.height/2.f * 0.5f;
	animation_frame_w = 1 / 4.f;
	animation_frame_h = 1 / 2.f;

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

	// Setting initial scale values
	m_size = { static_cast<float>(boss_texture.width) / 4.f, static_cast<float>(boss_texture.height) / 2.f };
	m_scale.x = 2.f;
	m_scale.y = 2.f;
	m_mass = 60.f;
	m_time_elapsed = 0.f;
	frame = 0;
	animation_dir = left;
	return true;
}

void Boss::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);
	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Boss::draw_current(const mat3& projection, const mat3& current_transform)
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
	glBindTexture(GL_TEXTURE_2D, boss_texture.id);
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
void Boss::update_current(float ms) {
	float time_factor = ms / 1000;
	if (m_vel.x != 0 || m_vel.y != 0) {
		m_time_elapsed += ms;
		if (m_time_elapsed > MS_PER_FRAME) {
			m_time_elapsed = 0;
			frame = (frame + 1) % NUM_FRAMES;
		}
	}
	//can move by using: 
	//apply_force_dv(vec2 change_in_velocity, float time) 
	//apply_force_dx(vec2 change_in_pos, float time)
	//the force corresponding to change in those values after "time" will be applied for "time"
	m_accel.x += m_external_force.x / m_mass;
	m_accel.y += m_external_force.y / m_mass;
	m_vel.x += m_accel.x * time_factor;
	m_vel.y += m_accel.y * time_factor;
	m_position.x += m_vel.x * time_factor;
	m_position.y += m_vel.y * time_factor;
}
void Boss::pick_movement_tex() {
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
	if (theta > pi / 2 || theta < -pi / 2)
		animation_dir = left;

}