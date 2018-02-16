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

  mat3 get_projection(GLFWwindow* window) const;
  mat3 get_transform();

  void draw(const mat3& projection, const mat3& parent_transform) override {}

private:
  const GameObject* m_follow;
};