#pragma once

#include "common.hpp"
#include "gameobject.hpp"
#include "healthbar.hpp"

#include <vector>

class GameObject;

struct Camera : public GameObject
{
public:
  Camera();
  ~Camera();

  bool init(int w, int h);
  bool init() override { return init(0, 0); }
  void destroy() override;
  
  void follow_object(const GameObject* object);
  const GameObject* get_m_follow();
  void stop_following();

  // Pass in window width and height
  mat3 get_projection() const;
  mat3 get_transform() const;

  void set_window_size(int w, int h) { m_window_width = w; m_window_height = h; }

  // UI Children
  HealthBar* getHealthBar() { return &m_healthBar; }

private:
  void update_current(float ms) override;
  void update_children(float ms) override; // change to update children for UI
  void draw_current(const mat3 &projection,
                    const mat3 &current_transform) override {}
  void draw_children(const mat3 &projection, const mat3 &current_transform) override; // change to draw children for UI

private:
  const GameObject* m_follow;
  int m_window_width, m_window_height; // width and height of render window, in pixels

  HealthBar m_healthBar;
};
