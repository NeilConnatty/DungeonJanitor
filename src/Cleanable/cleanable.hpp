#pragma once

#include "../common.hpp"
#include "../gameobject.hpp"

#include <vector>

class Cleanable : public GameObject
{

public:
	enum types
	{
		PUDDLE = 0,
		GRAFFITI = 1,
		GARBAGE = 2
	};
public:
	Cleanable();
	~Cleanable();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	virtual Texture& get_texture() = 0;
	virtual bool load_texture() = 0;
	virtual bool clean() = 0;

private:
	void update_current(float ms) override {};
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
