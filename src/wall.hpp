#pragma once

#include "common.hpp"
#include "gameobject.hpp"

class Wall : public GameObject
{
  static Texture bottom_vert_t;
  static Texture bottom_t;
  static Texture top_t;
  static Texture vert_t;
  static Texture corner_t;
  static Texture horz_t;

public:
  static vec2 get_dimensions(wall_edge edge);

	Wall();
	~Wall();

  bool  init() override { return init({ 0.f, 0.f }, NONE); };
	bool	init(vec2 position, wall_edge m_edge);
	void	destroy();

  void  set_wall_edge(wall_edge m_edge);
	vec2	get_bounding_box() const;

private:
	void update_current(float ms) override {} // no need for wall to be updated
	void update_children(float ms) override {}
	void draw_current(const mat3& projection, const mat3& current_transform) override;
	void draw_children(const mat3& projection, const mat3& current_transform) override;

  // these methods require precondition that m_texture be set (i.e. != nullptr)
  void set_vertices_top_bottom(TexturedVertex* vertices, size_t& numVertices, uint16_t* indices, size_t& numIndices);
  void set_vertices_bottom_vert(TexturedVertex* vertices, size_t& numVertices, uint16_t* indices, size_t& numIndices);
  void set_vertices_top_vert(TexturedVertex* vertices, size_t& numVertices, uint16_t* indices, size_t& numIndices);

  wall_edge m_edge;
  Texture*  m_texture;
};
