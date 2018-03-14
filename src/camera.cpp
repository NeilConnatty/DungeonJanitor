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

mat3 Camera::get_projection(int w, int h) const
{
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

mat3 Camera::get_transform(int w, int h)
{
  if (m_follow == nullptr)
  {
    transform_begin();
    transform_translate({ 250.f, 250.f });
    transform_end();

    return inverse(transform);
  }

  vec2 followPos = m_follow->get_pos();

  transform_begin();
  transform_translate({ followPos.x - (w / 2), followPos.y - (h / 2) });
  transform_end();

  return inverse(transform);
}

void Camera::follow_object(const GameObject* object)
{
  m_follow = object;
}
const GameObject* Camera::get_m_follow() { return m_follow; }
void Camera::stop_following()
{
  m_follow = nullptr;
}
