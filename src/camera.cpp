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

bool Camera::init(int w, int h)
{
	m_window_width = w; 
	m_window_height = h; 
	m_healthBar.init({ (float)w, (float)h });
	return true;
}

void Camera::destroy()
{
	m_healthBar.destroy();
}

mat3 Camera::get_projection() const
{
  // Fake projection matrix, scales with respect to window coordinates
  // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
  float left = 0.f;// *-0.5;
  float top = 0.f;// (float)h * -0.5;
  float right = (float)m_window_width;// *0.5;
  float bottom = (float)m_window_height;// *0.5;

  float sx = 2.f / (right - left);
  float sy = 2.f / (top - bottom);
  float tx = -(right + left) / (right - left);
  float ty = -(top + bottom) / (top - bottom);
  return { { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
}

void Camera::update_current(float ms)
{
  if (m_follow == nullptr)
  {
    m_position = { 250.f, 250.f };
    return;
  }

  vec2 followPos = m_follow->get_pos();
  m_position = { followPos.x - (m_window_width / 2), followPos.y - (m_window_height / 2) };
}

void Camera::update_children(float ms)
{
	m_healthBar.update(ms);
}

void Camera::draw_children(const mat3 & projection, const mat3 & current_transform)
{
	m_healthBar.draw(projection, current_transform);
}

mat3 Camera::get_transform() const
{
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
