// floor.cpp

#include "emitter.hpp"
#include <iostream>

static constexpr GLfloat k_vertex_buffer_data[] = {
 -0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
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
	m_scale = {100.f,100.f};
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, k_vertex_buffer_data, GL_STATIC_DRAW);

	// VBO for particle vertex translations
	glGenBuffers(1, &data.vbo_translation);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_translation);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 2 * sizeof(float), NULL, GL_STATIC_DRAW);

	// VBO for particle colors
	glGenBuffers(1, &data.vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_color);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(float), NULL, GL_STATIC_DRAW);

	glGenVertexArrays(1, &data.vao);
	glBindVertexArray(data.vao);

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
	//m_position = m_position + m_velocity * ms;
}

void Emitter::update_children(float ms) 
{
	if (m_particle_count < m_max_particles)
	{
		m_particle_container.emplace_back(m_position, m_velocity, m_color, m_lifetime);
		m_particle_count++;
		// std::cout << "m_position: " << m_position.z << std::endl;
		// std::cout << "m_velocity: " << m_velocity << std::endl;
		// std::cout << "m_color: " << m_color << std::endl;
		std::cout << "m_max_particles: " << m_max_particles << std::endl;
		std::cout << "size: " << m_particle_count << std::endl;
		std::cout << "m_particle_count: " << m_particle_count << std::endl;
	}

	// Iterate over all particles in the vector
	std::vector<Particle>::iterator p;
	for (p = m_particle_container.begin(); p != m_particle_container.end(); ++p)
	{
		p->p_life -= ms;
		// If the particle time to live is more than zero...
		if (p->p_life > 0)
		{
			vec2 dxdy = p->p_velocity * ms;
			p->p_position = p->p_position + dxdy;
			data.m_particles_translations.push_back(ms);
			data.m_particles_translations.push_back(ms);
		}
		else
		{
			// p = m_particle_container.erase(p);
			// m_particle_count--;
		}

	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_translation);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(float) * 2, data.m_particles_translations.data());

	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_color);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(float) * 4, data.m_particles_colors.data());
	}
}

void Emitter::draw_children(const mat3& projection, const mat3& current_transform)
{
 	// std::cout << m_particle_count << std::endl;
	glUseProgram(effect.program);
	glBindVertexArray(data.vao);

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
	glVertexAttribPointer(
		1, // attribute. No particular reason for 1, but must match the layout in the shader.
		2, // size : x + y + z + size => 4
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

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

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 2);
}

