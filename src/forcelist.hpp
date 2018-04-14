#pragma once

#include "common.hpp"
#include <iostream>
class ForceList
{
public:
	ForceList();
	~ForceList();
	int size;
	//constructors to add forces
	//how to delete nodes?
	//Force Active for dt in milliseconds
	struct ActiveForce
	{
		vec2 force;
		float dt;
		bool applied;
		ActiveForce() { 
			next = nullptr;
			applied = false;
		}
		ActiveForce(vec2 force_init, float dt_init) {
			force = force_init;
			dt = dt_init;
			next = nullptr;
			applied = false;
		}
		~ActiveForce() {}
		void tick(float ms) { dt -= ms; }
		ActiveForce* next;
	};
	
	void insert_back(ActiveForce newF);
	void delete_head();
	void delete_tail();
	void delete_pos(int pos);
	int get_size() { return size; }
	ActiveForce* head;
	ActiveForce* tail;
};