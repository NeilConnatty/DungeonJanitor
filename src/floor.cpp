// floor.cpp

#include "floor.hpp"

Floor::FloorTextures Floor::floor_textures;

vec2 Floor::get_dimensions()
{
  return { 35.f, 25.f };
}

Floor::Floor() {}

Floor::~Floor() {}

bool Floor::init()
{
	return init({ 0.f, 0.f }, HALLWAY_ROOM);
}

bool Floor::init(vec2 position, room_type type)
{
	if (!floor_textures.bath_textures[0].is_valid())
	{
    if (!load_textures())
    {
      fprintf(stderr, "Failed to load floor texture\n");
			return false;
    }
	}

  // C++ rng
  std::default_random_engine m_rng;
  std::uniform_int_distribution<int> m_dist; // default 0..1

  m_rng = std::default_random_engine(std::random_device()());
	
	m_position = position;
  m_room_type = type;

	// The position corresponds to the center of the texture
  float wr, hr;
  switch (m_room_type)
  {
  case BATH_ROOM:
    wr = floor_textures.bath_textures[1].width *
         0.5f; // some of the bath textures are of 1 pixel size difference..
               // using index 1 fixes some weird holes
    hr = floor_textures.bath_textures[0].height * 0.5f;
    m_dist = std::uniform_int_distribution<int>(0, NUM_BATH_TEXTURES - 1);
    break;
  case CLASS_ROOM:
    wr = floor_textures.class_textures[0].width * 0.5f;
    hr = floor_textures.class_textures[0].height * 0.5f;
    m_dist = std::uniform_int_distribution<int>(0, NUM_CLASS_TEXTURES - 1);
    break;
  case HALLWAY_ROOM:
    wr = floor_textures.hall_texture[0].width * 0.5f;
    hr = floor_textures.hall_texture[0].height * 0.5f;
    m_dist = std::uniform_int_distribution<int>(0, NUM_HALL_TEXTURES - 1);
    break;
  case OFFICE_ROOM:
    wr = floor_textures.office_textures[0].width * 0.5f;
    hr = floor_textures.office_textures[0].height * 0.5f;
    m_dist = std::uniform_int_distribution<int>(0, NUM_OFFICE_TEXTURES - 1);
    break;
  }

  m_texture_index = m_dist(m_rng);

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

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

bool Floor::load_textures()
{
  return  floor_textures.hall_texture[0].load_from_file(textures_path("dungeon1/d1_floortile_hall-na-1.png")) &&
    floor_textures.hall_texture[1].load_from_file(textures_path("dungeon1/d1_floortile_hall-na-2.png")) &&
    floor_textures.hall_texture[2].load_from_file(textures_path("dungeon1/d1_floortile_hall-na-3.png")) &&
    floor_textures.hall_texture[3].load_from_file(textures_path("dungeon1/d1_floortile_hall-na-4.png")) &&
    floor_textures.bath_textures[0].load_from_file(textures_path("dungeon1/d1_floortile_bathroom-1.png")) &&
    floor_textures.bath_textures[1].load_from_file(textures_path("dungeon1/d1_floortile_bathroom-2.png")) &&
    floor_textures.bath_textures[2].load_from_file(textures_path("dungeon1/d1_floortile_bathroom-3.png")) &&
    floor_textures.bath_textures[3].load_from_file(textures_path("dungeon1/d1_floortile_bathroom-4.png")) &&
    floor_textures.class_textures[0].load_from_file(textures_path("dungeon1/d1_floortile_class-1.png")) &&
    floor_textures.class_textures[1].load_from_file(textures_path("dungeon1/d1_floortile_class-2.png")) &&
    floor_textures.class_textures[2].load_from_file(textures_path("dungeon1/d1_floortile_class-3.png")) &&
    floor_textures.class_textures[3].load_from_file(textures_path("dungeon1/d1_floortile_class-4.png")) &&
    floor_textures.office_textures[0].load_from_file(textures_path("dungeon1/d1_floortile_office-na-1.png"));
}

void Floor::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Floor::draw_children(const mat3& projection, const mat3& current_transform)
{
}

void Floor::draw_current(const mat3& projection, const mat3& current_transform)
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
  GLuint id;
  switch (m_room_type)
  {
  case HALLWAY_ROOM:
    id = floor_textures.hall_texture[m_texture_index].id;
    break;
  case BATH_ROOM:
    id = floor_textures.bath_textures[m_texture_index].id;
    break;
  case CLASS_ROOM:
    id = floor_textures.class_textures[m_texture_index].id;
    break;
  case OFFICE_ROOM:
    id = floor_textures.office_textures[m_texture_index].id;
    break;
  }

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
