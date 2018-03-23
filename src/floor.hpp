#pragma once

#include "common.hpp"
#include "gameobject.hpp"

#include <random>

#define NUM_BATH_TEXTURES 4
#define NUM_CLASS_TEXTURES 4
#define NUM_HALL_TEXTURES 4
#define NUM_OFFICE_TEXTURES 1

class Floor : public GameObject
{
private:
  struct FloorTextures
  {
    Texture bath_textures[NUM_BATH_TEXTURES];
    Texture class_textures[NUM_CLASS_TEXTURES];
    Texture hall_texture[NUM_HALL_TEXTURES];
    Texture office_textures[NUM_OFFICE_TEXTURES];
  };

	static FloorTextures floor_textures;

public:
  static vec2 get_dimensions();

	Floor();
	~Floor();

	bool	init();
	bool	init(vec2 position, room_type type);
	void	destroy();
	
private:
	void update_current(float ms) override {} // no need for floor to be updated
	void update_children(float ms) override {}
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

  bool load_textures();

private:
  room_type m_room_type;
  size_t m_texture_index;
};
