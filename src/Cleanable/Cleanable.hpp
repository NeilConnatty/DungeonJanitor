#pragma once

#include "../common.hpp"
#include "../gameobject.hpp"

#include <vector>

class Cleanable : public GameObject
{
public:
	Cleanable();
	~Cleanable();

	bool init();
	void destroy();

private:
};
