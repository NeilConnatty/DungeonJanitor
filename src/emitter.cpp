// floor.cpp

#include "emitter.hpp"
#include <iostream>

static constexpr GLfloat k_vertex_buffer_data[] = {
 -0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 0.5f, 0.5f, 0.0f,
};

Emitter::Emitter() {}

Emitter::~Emitter() {}

bool Emitter::init() 
{
	return false;
}

bool Emitter::init(vec2 position, vec2 velocity, vec4 color, float lifetime, int max_particles)
{
	m_scale = {10.f,10.f};
	m_position = position;
	m_velocity = velocity;
	m_color = color; 
	m_lifetime = lifetime;
	m_max_particles = max_particles;
	m_particle_count = 0;

	data.m_particles_positions.resize(max_particles);
	data.m_particles_translations.resize(max_particles);
	data.m_particles_colors.resize(max_particles);

	// Clearing errors
	gl_flush_errors();

	// Loading shaders
	if (!effect.load_from_file(shader_path("particle.vs.glsl"), shader_path("particle.fs.glsl")))
		return false;

	// VBO for particle shape
	glGenBuffers(1, &data.vbo_shape);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_shape);
	glBufferData(GL_ARRAY_BUFFER, sizeof(k_vertex_buffer_data), k_vertex_buffer_data, GL_STATIC_DRAW);

	// VBO for particle vertex translations
	glGenBuffers(1, &data.vbo_translation);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_translation);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// VBO for particle colors
	glGenBuffers(1, &data.vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_color);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	return true;
}

void Emitter::destroy()
{
	std::cout << "escape" << std::endl;
	glDeleteBuffers(1, &data.vbo_shape);
	glDeleteBuffers(1, &data.vbo_translation);
	glDeleteBuffers(1, &data.vbo_color);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

// void Emitter::spawn_particles()
// {

// }

void Emitter::update_current(float ms)
{
	//m_position = m_position + m_velocity * (ms/1000.0f);

	if (m_particle_count < m_max_particles)
	{
		vec3 p_position = {m_position.x, m_position.y, 0.0f};
		vec3 p_velocity = {m_velocity.x, m_velocity.y, 0.0f};
		m_particle_container.emplace_back(p_position, p_velocity, m_color, m_lifetime);
		m_particle_count++;
	}
}

void Emitter::update_children(float ms) 
{
	// Iterate over all particles in the vector
	int next = 0;
	for (int p = 0; p < m_particle_count; ++p)
	{
		if (m_particle_container[p].p_life > 0.0f)
		{
			m_particle_container[p].update(ms/1000.0f);
			data.m_particles_translations[next + 0] = m_particle_container[p].p_position.x;
			data.m_particles_translations[next + 1] = m_particle_container[p].p_position.y;
			data.m_particles_translations[next + 2] = m_particle_container[p].p_position.z;
			data.m_particles_colors[next + 0] = m_particle_container[p].p_color.x;
			data.m_particles_colors[next + 1] = m_particle_container[p].p_color.y;
			data.m_particles_colors[next + 2] = m_particle_container[p].p_color.z;
			data.m_particles_colors[next + 3] = m_particle_container[p].p_color.w;
		}
		else 
		{
			m_particle_container.erase(m_particle_container.begin() + p + 1);
			p--;
			m_particle_count--;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_translation);
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(GLfloat) * 3, data.m_particles_translations.data());

	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_color);
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(GLfloat) * 4, data.m_particles_colors.data());
	
}

void Emitter::draw_children(const mat3& projection, const mat3& current_transform)
{
 	// std::cout << m_particle_count << std::endl;
	glUseProgram(effect.program);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_shape);
	glVertexAttribPointer(
		0, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_translation);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)01);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_color);
	glVertexAttribPointer(
		2, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : r + g + b + a => 4
		GL_FLOAT, // type
		GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0, // stride
		(void*)0 // array buffer offset
	);

	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	glVertexAttribDivisor(0, 0); 
	glVertexAttribDivisor(1, 1); 
	glVertexAttribDivisor(2, 1); 

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, m_particle_count);
}

