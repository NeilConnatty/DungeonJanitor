#pragma once

#include "../common.hpp"
#include "cleanable.hpp"

#include <stdlib.h>
#include <ctime>
#include <vector>
using namespace std;

class Graffiti : public Cleanable
{

private:
	static Texture graffiti_texture;

public:
	Graffiti();
	~Graffiti();
	bool init(vec2 pos);
	Texture& get_texture() override;
	bool load_texture() override;
};
