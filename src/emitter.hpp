#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "particle.hpp"
#include <vector>

class Emitter : public GameObject
{
public:

	Emitter();
	~Emitter();

	bool init() override;
	bool init(vec2 position, vec2 velocity, vec4 color, float lifetime, int max_particles);
	void destroy() override;
	
protected:
	void update_current(float ms) override {}
    void update_children(float ms) override;
    void draw_current(const mat3& projection, const mat3& current_transform) override {}
    void draw_children(const mat3& projection, const mat3& current_transform) override;

    struct DataGPU
	{
		std::vector<float> m_particles_positions; // container for emitters particles.
    	std::vector<float> m_particles_translations; // container for emitters particles.
    	std::vector<float> m_particles_colors; // container for emitters particles.
		GLuint vbo_pos;
		GLuint vbo_translation;
		GLuint vbo_color;
		GLuint vao;
	};

private:

	// vec2 m_position - position of spawn point.
	// float m_scale - intial size of particles. 
	vec2 m_velocity; // initial speed of particles.
	vec4 m_color; // color of particles.
	float m_lifetime; // total lifetime of a particle.
	int m_max_particles; // max awake particles.
	int m_particle_count; // num active particles.
	GameObject* attached_to; // gameobject emitter is attached to.
	std::vector<Particle> m_particle_container; // container for emitters particles.
	DataGPU data;
};