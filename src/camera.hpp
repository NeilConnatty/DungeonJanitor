#pragma once

#include "common.hpp"

class GameObject;

struct Camera : public Renderable
{
public:
  Camera();
  ~Camera();
  
  void follow_object(const GameObject* object);
  void stop_following();

  // Pass in window width and height
  mat3 get_projection(int w, int h) const;
  mat3 get_transform(int w, int h);

  void draw(const mat3& projection, const mat3& parent_transform) override {}

private:
  const GameObject* m_follow;
};
