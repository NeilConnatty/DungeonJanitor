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
	bool init(vec2 position, vec2 velocity, vec2 acceleration, vec4 color, float scale, float angle, float weight, float lifetime, int max_particles);
	void destroy() override;
	bool attach_gameobject(GameObject object) {}
	int find_unused_particle();
	void init_particles();

	struct Particle
	{	
		Particle() {p_life = -1.0f;}
		
		vec2 p_position;
		vec2 p_velocity;
		vec2 p_acceleration;
		vec4 p_color;
		float p_scale;
		float p_alpha;
		float p_angle;
		float p_weight;
		float p_life; // curr life, < 0 : dead and unused.
		bool p_alive;
	};
	
protected:
	void update_current(float ms) override;
    void update_children(float ms) override;
    void draw_current(const mat3& projection, const mat3& current_transform) override {}
    void draw_children(const mat3& projection, const mat3& current_transform) override;
    
    struct Mesh
	{
		GLfloat vbo;
		GLfloat ibo;
		GLfloat vao;
	};

private:

	// vec2 m_position - position of spawn point.
	// float m_scale - intial size of particles. 
	vec2 m_velocity; // initial speed of particles.
	vec2 m_acceleration; // initial acceleration of particles.
	vec4 m_color; // color of particles.
	float m_angle; // angle of spawn.
	float m_weight; // initial weight of particles.
	float m_lifetime; // total lifetime.
	int m_max_particles; // max awake particles.
	int m_particle_count; // num active particles.
	int m_nextparticle; // next free particle index in container.
	GameObject* attached_to; // gameobject emitter is attached to.
	std::vector<Particle> m_particle_container; // container for emitters particles.
};