#pragma once

#include "common.hpp"
#include <iostream>

class Particle
{
public:
	Particle();
	Particle(vec2 position, float time);
	~Particle();
	void update(float ms);

	vec2 get_position() {return m_position;}
	vec2 get_velocity() {return m_velocity;}
	vec4 get_color() {return m_color;}
	float get_alive_time() {return m_life;}
	void set_position(vec2 pos) {m_position = pos;}
	void set_speed(vec2 velocity) {m_velocity = velocity;}
	void set_color(vec4 color) {m_color = color;}
  	void set_alive_time(float time) {m_life = time;}
 	//void decrement_alive_time(float timestep) {m_life -= timestep;

protected:
	vec2 m_position;
	vec2 m_velocity;
	vec4 m_color;
	float m_life; // curr life, < 0 : dead and unused.
};

