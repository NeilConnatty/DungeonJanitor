#pragma once

#include "../common.hpp"
#include "../gameobject.hpp"

#include <vector>
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

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

  virtual Mix_Chunk& get_sound() = 0;
  virtual void play_sound(){
    m_sound = &get_sound();
    Mix_PlayChannel(-1, m_sound, 0);
  };
  Mix_Chunk* m_sound;

private:
	void update_current(float ms) override {};
	void update_children(float ms) override {};
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
