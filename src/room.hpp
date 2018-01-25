#pragma once

#include "common.hpp"

class Room : Renderable
{
public:
	Room();
	~Room();

	// Creates a room
	bool init();

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our room
	void draw(const mat3& projection) override;

private:
};