#pragma once

#include "../common.hpp"
#include "cleanable.hpp"

#include <vector>

class Puddle : public Cleanable
{

private:
	static Texture puddle_texture;

public:
	Puddle();
	~Puddle();
  bool init(vec2 pos);
	Texture& get_texture() override;
	bool load_texture() override;
	bool clean() override;
};
