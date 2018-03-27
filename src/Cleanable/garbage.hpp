#pragma once

#include "../common.hpp"
#include "cleanable.hpp"

#include <vector>
#include <random>
using namespace std;

#define NUM_GARBAGE_TEXTURES 2

class Garbage : public Cleanable
{

private:
	static Texture garbage_texture_blue[NUM_GARBAGE_TEXTURES];
	static Texture garbage_texture_green[NUM_GARBAGE_TEXTURES];
	bool m_is_empty;
	int m_texture_index;

public:
	Garbage();
	~Garbage();
	bool init(vec2 pos);
	Texture& get_texture() override;
	bool load_texture() override;
	bool clean() override;
};
