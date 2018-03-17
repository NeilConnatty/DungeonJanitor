#pragma once

#include "../common.hpp"
#include "cleanable.hpp"

#include <vector>

class Graffiti : public Cleanable
{

private:
	static Texture graffiti_texture;

public:
	Graffiti();
	~Graffiti();
	Texture& get_texture() override;
	bool load_texture() override;
};
