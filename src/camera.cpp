// camera.cpp

#include "camera.hpp"
#include "gameobject.hpp"

Camera::Camera() :
  m_follow(nullptr)
{
}

Camera::~Camera() 
{
}

mat3 Camera::get_projection(GLFWwindow* window) const
{
  // Getting size of window
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  // Fake projection matrix, scales with respect to window coordinates
  // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
  float left = 0.f;// *-0.5;
  float top = 0.f;// (float)h * -0.5;
  float right = (float)w;// *0.5;
  float bottom = (float)h;// *0.5;

  float sx = 2.f / (right - left);
  float sy = 2.f / (top - bottom);
  float tx = -(right + left) / (right - left);
  float ty = -(top + bottom) / (top - bottom);
  return { { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
}

mat3 Camera::get_transform() 
{
  if (m_follow == nullptr)
  {
    return transform;
  }

  transform_begin();
  transform_translate(m_follow->get_pos());
  transform_end();

  return transform;
}

void Camera::follow_object(const GameObject* object)
{
  m_follow = object;
}

void Camera::stop_following()
{
  m_follow = nullptr;
}