#pragma once

#include "../common.hpp"
#include "../gameobject.hpp"

#include <vector>

class Puddle : public GameObject
{
public:
	Puddle();
	~Puddle();
	
private:
	//void update_current(float ms) override {} 
	//void draw_current(const mat3& projection, const mat3& current_transform) override;
};
