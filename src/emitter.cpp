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
	m_position = position;
	m_velocity = velocity;
	m_color = color; 
	m_lifetime = lifetime;
	m_max_particles = max_particles;
	m_particle_count = 0;

	// Clearing errors
	gl_flush_errors();

	// Loading shaders
	if (!effect.load_from_file(shader_path("particle.vs.glsl"), shader_path("particle.fs.glsl")))
		return false;

	// VBO for particle vertices
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(k_vertex_buffer_data), k_vertex_buffer_data, GL_STATIC_DRAW);

	// VBO for particle positions
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.ibo);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// VBO for particle colors
	glGenBuffers(1, &mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vao);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	return true;
}

void Emitter::destroy()
{
	// glDeleteBuffers(1, &mesh.vbo);
	// glDeleteBuffers(1, &mesh.ibo);
	// glDeleteBuffers(1, &mesh.vao);

	// glDeleteShader(effect.vertex);
	// glDeleteShader(effect.fragment);
	// glDeleteShader(effect.program);
}

// void Emitter::update_current(float ms)
// {
// 	std::cout << "size: " << m_particle_container.size() << std::endl;
// 	if (m_particle_count < m_max_particles)
// 	{
// 		m_particle_container.emplace_back(m_position, m_lifetime);
// 		m_particle_count++;
// 	}
// }

void Emitter::update_children(float ms) 
{
	std::cout << "size: " << m_particle_container.size() << std::endl;
	if (m_particle_count < m_max_particles)
	{
		m_particle_container.emplace_back(m_position, m_lifetime);
		m_particle_count++;
	}
	// Iterate over all particles in the vector
	std::vector<Particle>::iterator i;
	for (i = m_particle_container.begin(); i != m_particle_container.end(); ++i)
	{
		// If the particle time to live is more than zero...
		if (i->get_alive_time() > 0)
		{
			// ...update the particle position position and draw it.
			i->update(ms);
		}
		else // // If it's time to destroy the particle...
		{
			// ...then remove it from the vector...
			// NOTE: Calling erase(SOME-ELEMENT) removes the element from the vector and
			// returns the next element in the vector, even if it's the "one-past-the-last-element"
			// i.e. "end()" element which signifies the end of the vector.
			//
			// It's VERY important to assign this next element back to the iterator 
			// because we're modyfing the length of the vector inside a loop which goes
			// until it gets to the end of a vector --- which, ya know, we're actually changing
			// as we go! If you don't assign the next element back to the iterator when calling
			// erase - don't be surprised when your code bombs out with a segfault, k? ;-D

			i = m_particle_container.erase(i);
 
			// ...and then add a new particle to replace it!
			m_particle_container.emplace_back(m_position, m_lifetime);
		}

	// glBindBuffer(GL_ARRAY_BUFFER, mesh.ibo);
	// glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	// glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(GLfloat) * 4, m_particle_positions_data);

	// glBindBuffer(GL_ARRAY_BUFFER, mesh.vao);
	// glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	// glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(GLfloat) * 4, m_particle_colors_data);
}
}

void Emitter::draw_children(const mat3& projection, const mat3& current_transform)
{
	// // 1rst attribute buffer : vertices
	// glEnableVertexAttribArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// glVertexAttribPointer(
	// 	 0, // attribute. No particular reason for 0, but must match the layout in the shader.
	// 	 3, // size
	// 	 GL_FLOAT, // type
	// 	 GL_FALSE, // normalized?
	// 	 0, // stride
	// 	 (void*)0 // array buffer offset
	// );

	// // 2nd attribute buffer : positions of particles' centers
	// glEnableVertexAttribArray(1);
	// glBindBuffer(GL_ARRAY_BUFFER, mesh.ibo);
	// glVertexAttribPointer(
	// 	 1, // attribute. No particular reason for 1, but must match the layout in the shader.
	// 	 4, // size : x + y + z + size => 4
	// 	 GL_FLOAT, // type
	// 	 GL_FALSE, // normalized?
	// 	 0, // stride
	// 	 (void*)0 // array buffer offset
	// );

	// // 3rd attribute buffer : particles' colors
	// glEnableVertexAttribArray(2);
	// glBindBuffer(GL_ARRAY_BUFFER, mesh.vao);
	// glVertexAttribPointer(
	// 	 2, // attribute. No particular reason for 1, but must match the layout in the shader.
	// 	 4, // size : r + g + b + a => 4
	// 	 GL_FLOAT, // type
	// 	 GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
	// 	 0, // stride
	// 	 (void*)0 // array buffer offset
	// );

	// glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	// glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	// glVertexAttribDivisor(2, 1); // color : one per quad -> 1

	// std::cout << m_particle_count << std::endl;

	// glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particle_count);
}

