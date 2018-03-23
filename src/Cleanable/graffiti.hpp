#pragma once

#include "../common.hpp"
#include "cleanable.hpp"

#include <stdlib.h>
#include <ctime>
#include <vector>
#include <random>

using namespace std;

#define NUM_GRAFFITI_TEXTURES 3

class Graffiti : public Cleanable
{

private:
	static Texture graffiti_textures[NUM_GRAFFITI_TEXTURES];
	int m_texture_index;

public:
	Graffiti();
	~Graffiti();
	bool init(vec2 pos);
	Texture& get_texture() override;
	bool load_texture() override;
	bool clean() override;
};
