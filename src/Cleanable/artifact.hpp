#pragma once

#include "../common.hpp"
#include "../gameobject.hpp"

#include <vector>
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class Artifact : public GameObject
{

private:
	static Texture activated_artifact_texture;
	static Texture deactivated_artifact_texture;
	bool m_is_activated;

public:
	Artifact();
	~Artifact();

	bool	init();
	bool	init(vec2 position);
	void	destroy();
	bool	is_activated();
	void  set_active(bool active);
  Mix_Chunk* m_sound;

private:
	void update_current(float ms) override {};
	void update_children(float ms) override {};  
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
