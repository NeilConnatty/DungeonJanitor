// wall.cpp

#include "wall.hpp"

Texture Wall::bottom_vert_t;
Texture Wall::bottom_t;
Texture Wall::top_t;
Texture Wall::vert_t;
Texture Wall::horz_t;
Texture Wall::corner_t;

vec2 Wall::get_dimensions(wall_edge edge)
{
  vec2 pos;
  if ((edge & (VERTICAL | BOTTOM)) == (VERTICAL | BOTTOM) ||
      (edge & (VERTICAL | TOP)) == (VERTICAL | TOP)) 
  {
    pos = { 10.f, 60.f };
  }
  else if ((edge & VERTICAL) == VERTICAL)
  {
    pos = { 10.f, 25.f };
  }
  else if ((edge & BOTTOM) == BOTTOM || (edge & TOP) == TOP)
  {
    pos = { 35.f, 60.f };
  }
  else // edge is NONE, this is a weird case in the text parser, but the equivalent is BOTTOM
  {
    pos = { 35.f, 60.f };
  }
  return pos;
}

Wall::Wall() : m_texture(nullptr) {}

Wall::~Wall() {}

bool Wall::init(vec2 position, wall_edge edge)
{
	if (!bottom_vert_t.is_valid())
	{
		if (!bottom_vert_t.load_from_file(textures_path("dungeon1/d1_walltile_cement-strip-1.png")))
		{
			fprintf(stderr, "Failed to load wall texture\n");
			return false;
		}
    if (!bottom_t.load_from_file(textures_path("dungeon1/d1_walltile_cement.png")))
    {
      fprintf(stderr, "Failed to load wall texture\n");
      return false;
    }
    if (!top_t.load_from_file(textures_path("dungeon1/d1_walltile_cement.png")))
    {
      fprintf(stderr, "Failed to load wall texture\n");
      return false;
    }
    if (!vert_t.load_from_file(textures_path("dungeon1/d1_walltile_cement-top-v-1.png")))
    {
      fprintf(stderr, "Failed to load wall texture\n");
      return false;
    }
    if (!corner_t.load_from_file(textures_path("dungeon1/d1_walltile_cement-vert.png")))
    {
      fprintf(stderr, "Failed to load wall texture\n");
      return false;
    }
	}

	m_position = position;
  TexturedVertex vertices[8]; // we use a max of 8 vertices for this object
  uint16_t indices[18];       // max of 6 triangles (i.e. 6 triangles * 3 indices)
  size_t numVertices, numIndices = 0;

  if ((edge & (VERTICAL | BOTTOM)) == (VERTICAL | BOTTOM))
  {
    m_position.x = position.x - 12.5f;
    m_position.y = position.y + 17.5f;
    m_texture = &bottom_vert_t;
  }
  else if ((edge & (VERTICAL | TOP)) == (VERTICAL | TOP))
  {
    m_texture = &corner_t;
    m_position.y = position.y + 17.5f;
    m_position.x = position.x - 12.5f;
  }
  else if ((edge & VERTICAL) == VERTICAL)
  {
    m_position.x = position.x - 12.5f;
    m_texture = &vert_t;
  }
  else if ((edge & BOTTOM) == BOTTOM)
  {
    m_position.y = position.y + 17.5f;
    m_texture = &bottom_t;
  }
  else if ((edge & TOP) == TOP)
  {
    m_position.y = position.y + 17.5f;
    m_texture = &top_t;
  }

  if (!m_texture)
  {
    fprintf(stderr, "Failed to set m_texture\n");
    return false;
  }

  m_size = { static_cast<float>(m_texture->width), static_cast<float>(m_texture->height) };
  
  set_vertices_top_bottom(vertices, numVertices, indices, numIndices);

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * numVertices, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * numIndices, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial scale values
	m_scale.x = 1.f;
	m_scale.y = 1.f;

	return true;
}

void Wall::set_vertices_top_bottom(TexturedVertex *vertices,
                                   size_t &numVertices, uint16_t *indices,
                                   size_t &numIndices) 
{
  if (m_texture == nullptr)
  {
    return;
  }

  // The position corresponds to the center of the texture
  float wr = m_texture->width * 0.5f;
  float hr = m_texture->height * 0.5f;

  numVertices = 4;
  numIndices = 6;
  // counterclockwise as it's the default opengl front winding direction
  indices[0] = 0; indices[1] = 3; indices[2] = 1; 
  indices[3] = 1; indices[4] = 3; indices[5] = 2;

  vertices[0].position = { -wr, +hr, -0.02f };
  vertices[0].texcoord = { 0.f, 1.f };
  vertices[1].position = { +wr, +hr, -0.02f };
  vertices[1].texcoord = { 1.f, 1.f };
  vertices[2].position = { +wr, -hr, -0.02f };
  vertices[2].texcoord = { 1.f, 0.f };
  vertices[3].position = { -wr, -hr, -0.02f };
  vertices[3].texcoord = { 0.f, 0.f };
}

void Wall::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Wall::draw_children(const mat3& projection, const mat3& current_transform)
{
}

void Wall::draw_current(const mat3& projection, const mat3& current_transform)
{
	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
