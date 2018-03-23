#pragma once

#include "../common.hpp"
#include "cleanable.hpp"

#include <vector>

class Garbage : public Cleanable
{

private:
	static Texture garbage_texture_full;
	static Texture garbage_texture_empty;
	bool m_is_empty;

public:
	Garbage();
	~Garbage();
	bool init(vec2 pos);
	Texture& get_texture() override;
	bool load_texture() override;
	void clean() override;
};
