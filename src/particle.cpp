// particle.cpp

#include "particle.hpp" 

Particle::Particle()
{
	vec2 m_position = {0.0f, 0.0f};
	vec2 m_velocity = {1.0f, 0.0f};
	vec4 m_color = {1.0f, 0.0f, 0.0f, 1.0f};
	float m_life = -1.0f;
}

Particle::Particle(vec2 position, float lifetime)
{
	m_position = position;
	m_velocity = {1.0f, 0.0f};
	m_color = {1.0f, 0.0f, 0.0f, 1.0f};
	m_life = lifetime;
}

Particle::~Particle() {}

void Particle::update(float ms)
{
	vec2 delta_position = m_velocity * ms;
	vec2 new_position = m_position + delta_position;
	m_position = new_position;
		std::cout << "m_life: "<< m_life << std::endl;
	m_life -= ms;
		std::cout << "m_life: "<< m_life << std::endl;
}