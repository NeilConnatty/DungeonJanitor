#pragma once

#include "common.hpp"
#include "forcelist.hpp"
#include "vector"
#include <memory>
class GameObject : public Renderable
{
public:
  GameObject();
  ~GameObject();

  virtual bool init() = 0;
  virtual void destroy() = 0;

  void set_pos(vec2 pos);
  void set_scale(vec2 scale);
  void set_size(vec2 size);
  void set_rotation(float rotation);
  // void set_z(int newz);
  // can probably just evaluate this from m_position.y % tile_y_size
  vec2 get_pos() const;
  vec2 get_scale() const;
  vec2 get_size() const;
  float get_rot() const;
  bool is_enabled() const;

  void toggle_enable();
  void toggle_physics();
  void update(float ms);
  void draw(const mat3 &projection, const mat3 &parent_transform) override;
  bool collides_with(GameObject& object, mat3 room_transform, mat3 dungeon_transform);
  bool collides_with_projected(GameObject& object, vec2 projected_position, mat3 room_transform, mat3 dungeon_transform); // check collisions for projected position of game object

  void apply_force_dv(vec2 desired_vel, float dt);
  void apply_force_dx(vec2 dx, float dt);

protected:
  virtual void update_current(float ms) = 0;
  virtual void update_children(float ms) = 0;
  virtual void draw_current(const mat3 &projection,
                            const mat3 &current_transform) = 0;
  virtual void draw_children(const mat3 &projection,
                             const mat3 &current_transform) = 0;

protected:
  bool m_enabled;
  bool m_physics_object;

  int m_z;
  float m_rotation;
  float m_mass;
  vec2 m_external_force;
  vec2 m_accel;
  vec2 m_vel;
  vec2 m_position;
  vec2 m_scale;
  vec2 m_size;
  
  ForceList m_active_forces;
};
