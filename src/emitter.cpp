// floor.cpp

#include "emitter.hpp"
#include <iostream>

static GLfloat* m_particle_positions_data = new GLfloat[10];
static GLfloat* m_particle_colors_data = new GLfloat[10];
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

bool Emitter::init(vec2 position, vec2 velocity, vec2 acceleration, vec4 color, float scale, float angle, float weight, float lifetime, int max_particles)
{
	m_position = position;
	m_velocity = velocity;
	m_acceleration = acceleration;
	m_scale = {scale, scale}; 
	m_color = color; 
	m_angle = angle; 
	m_weight = weight;
	m_lifetime = lifetime;
	m_max_particles = max_particles;
	m_nextparticle = 0;
	m_particle_container.reserve(max_particles);

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
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Emitter::update_current(float ms)
{
	int newparticles = (int)(ms*1000.0);
	if (newparticles > (int)(0.002f*1000.0))
    	newparticles = (int)(0.002f*1000.0);
    for (int i = 0; i < newparticles; ++i)
    {
    	int next = find_unused_particle();
    	m_particle_container[next].p_life = m_lifetime;
		m_particle_container[next].p_position = m_position;
		m_particle_container[next].p_velocity = m_velocity;
		m_particle_container[next].p_acceleration = m_acceleration;
		m_particle_container[next].p_color = m_color;
		m_particle_container[next].p_scale = m_scale.x;
		m_particle_container[next].p_angle = m_angle; 
		m_particle_container[next].p_weight = m_weight;
		m_particle_container[next].p_alive = true;
    }
}

int Emitter::find_unused_particle() {
    for(int i = m_nextparticle; i < m_max_particles; i++){
        if (m_particle_container[i].p_life < 0.0f){
            m_nextparticle = i;
            return i;
        }
    }
    for(int i=0; i<m_nextparticle; i++){
        if (m_particle_container[i].p_life < 0.0f){
            m_nextparticle = i;
            return i;
        }
    }
    return 0; // All particles are taken, override the first one
}

void Emitter::update_children(float ms) 
{
	m_particle_count = 0;
	for(int i=0; i<m_max_particles; i++)
	{
	    Particle& p = m_particle_container[i]; // shortcut
	    if(p.p_life > 0.0f)
	    {
	        // Decrease life
	        p.p_life -= ms;
	        if (p.p_life > 0.0f)
	        {
	            // simple physics
	            p.p_position.x += p.p_velocity.x * (float)ms;
	            p.p_position.y += p.p_velocity.y * (float)ms;

	            // Fill the GPU buffer
	            m_particle_positions_data[4*m_particle_count+0] = p.p_position.x;
	            m_particle_positions_data[4*m_particle_count+1] = p.p_position.y;
	            m_particle_positions_data[4*m_particle_count+2] = -0.02f;
	            m_particle_positions_data[4*m_particle_count+3] = p.p_scale;

	            m_particle_colors_data[4*m_particle_count+0] = p.p_color.x;
	            m_particle_colors_data[4*m_particle_count+1] = p.p_color.y;
	            m_particle_colors_data[4*m_particle_count+2] = p.p_color.z;
	            m_particle_colors_data[4*m_particle_count+3] = p.p_color.w;

	            m_particle_count++;
	        }
	        else
	        {
	            p.p_alive = false;
	        }
    	}
	}

	glBindBuffer(GL_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(GLfloat) * 4, m_particle_positions_data);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vao);
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_count * sizeof(GLfloat) * 4, m_particle_colors_data);
}

void Emitter::draw_children(const mat3& projection, const mat3& current_transform)
{
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
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
	glBindBuffer(GL_ARRAY_BUFFER, mesh.ibo);
	glVertexAttribPointer(
		 1, // attribute. No particular reason for 1, but must match the layout in the shader.
		 4, // size : x + y + z + size => 4
		 GL_FLOAT, // type
		 GL_FALSE, // normalized?
		 0, // stride
		 (void*)0 // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vao);
	glVertexAttribPointer(
		 2, // attribute. No particular reason for 1, but must match the layout in the shader.
		 4, // size : r + g + b + a => 4
		 GL_FLOAT, // type
		 GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		 0, // stride
		 (void*)0 // array buffer offset
	);

	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad -> 1

	std::cout << m_particle_count << std::endl;

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particle_count);
}
