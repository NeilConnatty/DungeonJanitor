// healthbar.cpp

#include "healthbar.hpp"

HealthBar::HealthBar()
{
}

HealthBar::~HealthBar()
{
}

bool HealthBar::init()
{
	//if (!validate_textures()) return false;
	return false;
}

void HealthBar::destroy()
{
}

void HealthBar::draw_current(const mat3 & projection, const mat3 & current_transform)
{
}

void HealthBar::draw_children(const mat3 & projection, const mat3 & current_transform)
{
}

void HealthBar::update_current(float ms)
{
}

void HealthBar::update_children(float ms)
{
}
