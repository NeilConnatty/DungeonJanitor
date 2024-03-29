#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class FloorObject : public GameObject
{
public:
  FloorObject();
  ~FloorObject();

  virtual bool init();
  virtual bool init(vec2 position);
  void	destroy();
  virtual Texture& get_texture() = 0;
  virtual bool load_texture() = 0;

private:
  void update_current(float ms) override {};
  void update_children(float ms) override {};
  void draw_current(const mat3& projection, const mat3& current_transform) override;
  void draw_children(const mat3& projection, const mat3& current_transform) override {};
};
