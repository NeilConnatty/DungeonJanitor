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

bool Emitter::init(vec2 scale, vec2 position, vec2 velocity, vec4 color, float lifetime, int max_particles, vec2 spawn_frequency)
{
	m_scale = scale;
	m_position = position;
	m_velocity = velocity;
	m_color = color; 
	m_lifetime = lifetime;
	m_max_particles = max_particles;
	m_spawn_frequency = spawn_frequency;
	m_particle_count = 0;
	m_next_spawn = 0.0f;

	data.m_particles_positions.resize(max_particles*3);
	data.m_particles_translations.resize(max_particles*3);
	data.m_particles_colors.resize(max_particles*4);

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
	glDeleteBuffers(1, &data.vbo_shape);
	glDeleteBuffers(1, &data.vbo_translation);
	glDeleteBuffers(1, &data.vbo_color);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Emitter::update_current(float ms)
{
	//m_position = m_position + m_velocity * (ms/1000.0f);
	if (m_next_spawn < 0.0f) 
	{
		for (int i = 0; i < m_spawn_frequency.y; ++i)
		{	
			if (m_particle_count < m_max_particles) 
			{
			vec3 p_position = {m_position.x, m_position.y, 0.0f};
			vec3 p_velocity = {m_velocity.x, m_velocity.y, 0.0f};
			m_particle_container.emplace_back(p_position, p_velocity, m_color, m_lifetime);
			m_particle_count++;
			}
		}
		m_next_spawn = m_spawn_frequency.x;
	}
	else 
	{
		m_next_spawn = m_next_spawn - ms/1000.0f;
	}
}

void Emitter::update_children(float ms) 
{
	// Iterate over all particles in the vector
	int next = 0;
	for (int p = 0; p < m_particle_count; ++p)
	{
		if (m_particle_container[p].p_life >= 0.0f)
		{
			m_particle_container[p].update_shape_line(ms/1000.0f);
			data.m_particles_translations[3 * next + 0] = m_particle_container[p].p_position.x;
			data.m_particles_translations[3 * next + 1] = m_particle_container[p].p_position.y;
			data.m_particles_translations[3 * next + 2] = m_particle_container[p].p_position.z;
			data.m_particles_colors[4 * next + 0] = m_particle_container[p].p_color.x;
			data.m_particles_colors[4 * next + 1] = m_particle_container[p].p_color.y;
			data.m_particles_colors[4 * next + 2] = m_particle_container[p].p_color.z;
			data.m_particles_colors[4 * next + 3] = m_particle_container[p].p_color.w;
			next++;
		}
		else 
		{
			m_particle_container.erase(m_particle_container.begin() + p);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);	
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_translation);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, data.vbo_color);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	glVertexAttribDivisor(0, 0); 
	glVertexAttribDivisor(1, 1); 
	glVertexAttribDivisor(2, 1); 

	//debug_print_gpu();
	//debug_print_state();

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particle_count);
}

void Emitter::debug_print_state() 
{
	std::cout << "EMITTER STATUS ..." << std::endl;
	std::cout << "emitter position ...  (" << m_position.x << ", " << m_position.y << ", " << ")  ..." << std::endl;
	std::cout << "emitter velocity ...  (" << m_velocity.x << ", " << m_velocity.y << ", " << ")  ..." << std::endl;
	std::cout << "emitter color ...  (" << m_color.x << ", " << m_color.y << ", " << m_color.z << ", " << m_color.w << ")  ..." << std::endl;
	std::cout << "emitter lifetime ...  " << m_lifetime << std::endl;
	std::cout << "emitter max particles ...  " << m_max_particles << std::endl;
	std::cout << "emitter particle count ...  " << m_particle_count << std::endl;
	std::cout << "emitter next spawn timer ...  " << m_next_spawn << std::endl;
	std::cout << "PARTICLES STATUS ..." << std::endl;
	for (int p = 0; p < m_particle_count; ++p)
	{
		std::cout << "PARTICLE STATUS for PARTICLE ..." << p << std::endl;
		std::cout << "particle position ...  (" << m_particle_container[p].p_position.x << ", " << m_particle_container[p].p_position.y << ", " << m_particle_container[p].p_position.z << ")  ..." << std::endl;
		std::cout << "particle color ...  (" << m_particle_container[p].p_color.x << ", " << m_particle_container[p].p_color.y << ", " << m_particle_container[p].p_color.z << ", " << m_particle_container[p].p_color.w << ")  ..." << std::endl;
		std::cout << "particle life ...  " << m_particle_container[p].p_life << std::endl;
	}
}

void Emitter::debug_print_gpu()
{
	for (int p = 0; p < m_particle_count; ++p)
	{
		std::cout << "PARTICLE GPU STATUS for PARTICLE ..." << p << std::endl;
		std::cout << "particle translation ...  (" << data.m_particles_translations[3 * p + 0] << ", " << data.m_particles_translations[3 * p + 1] << ", " << data.m_particles_translations[3 * p + 2] << ")  ..." << std::endl;
		std::cout << "particle color ...  (" << data.m_particles_colors[4 * p + 0] << ", " << data.m_particles_colors[4 * p + 1] << ", " << data.m_particles_colors[4 * p + 2] << ", " << data.m_particles_colors[4 * p + 3] << ")  ..." << std::endl;
	}
}

