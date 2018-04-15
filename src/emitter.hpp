#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include <vector>

class Emitter : public GameObject
{
public:

	Emitter();
	~Emitter();

	bool init() override;
	bool init(vec2 scale, vec2 position, vec2 velocity, vec4 color, float lifetime, int max_particles, vec2 spawn_frequency);
	void destroy() override;
	
protected:
	void debug_print_state();
	void debug_print_gpu();
	void update_current(float ms) override;
    void update_children(float ms) override;
    void draw_current(const mat3& projection, const mat3& current_transform) override {}
    void draw_children(const mat3& projection, const mat3& current_transform) override;

    struct Particle
    {
		vec3 p_position;
		vec3 p_velocity;
		vec4 p_color;
		float p_life; // curr life, < 0 : dead and unused.

		Particle()
		{
			p_position = {0.0, 0.0, 0.0};
			p_velocity = {0.0, 0.0, 0.0};
			p_color = {1.0, 0.0, 0.0, 1.0};
			p_life = 0.0;
		}

		Particle(vec3 position, vec3 velocity, vec4 color, float life) 
		{
			p_position = position;
			p_velocity = velocity;
			p_color = color;
			p_life = life;
		}

		void update_shape_line(float ms)
		{
			p_position = {p_position.x + p_velocity.x * ms, p_position.y + p_velocity.y * ms, 0.0f};
			p_color = {p_color.x, p_color.y, p_color.z, p_color.w - 0.01f};
			p_life = p_life - ms;
		}

		void update_shape_spiral(float ms)
		{
			p_position = {p_position.x + p_velocity.x * ms, p_position.y + p_velocity.y * ms, 0.0f};
			p_color = {p_color.x, p_color.y, p_color.z, p_color.w - 0.01f};
			p_life = p_life - ms;			
		}

		void update_shape_circle(float ms)
		{
			p_position = {p_position.x + p_velocity.x * ms, p_position.y + p_velocity.y * ms, 0.0f};
			p_color = {p_color.x, p_color.y, p_color.z, p_color.w - 0.01f};
			p_life = p_life - ms;			
		}

		void update_shape_square(float ms)
		{
			p_position = {p_position.x + p_velocity.x * ms, p_position.y + p_velocity.y * ms, 0.0f};
			p_color = {p_color.x, p_color.y, p_color.z, p_color.w - 0.01f};
			p_life = p_life - ms;			
		}

		void update_shape_rand(float ms)
		{
			p_position = {p_position.x + p_velocity.x * ms, p_position.y + p_velocity.y * ms, 0.0f};
			p_color = {p_color.x, p_color.y, p_color.z, p_color.w - 0.01f};
			p_life = p_life - ms;			
		}
    };

    struct DataGPU
	{
		std::vector<float> m_particles_positions; 
    	std::vector<float> m_particles_translations; 
    	std::vector<float> m_particles_colors; 
		GLuint vbo_shape;
		GLuint vbo_translation;
		GLuint vbo_color;
	};

private:
	GameObject* attached_to; // gameobject emitter is attached to.
	vec2 m_velocity; // initial speed of particles.
	vec4 m_color; // color of particles.
	float m_lifetime; // total lifetime of a particle.
	int m_max_particles; // max awake particles.
	int m_particle_count; // num active particles.
	vec2 m_spawn_frequency; // frequency of spawning particles -> {time between spawn, number to spawn}
	float m_next_spawn;
	std::vector<Particle> m_particle_container; // container for emitters particles.
	DataGPU data;
};