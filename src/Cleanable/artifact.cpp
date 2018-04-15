// artifact.cpp

#include "artifact.hpp"

Texture Artifact::artifact_textures[NUM_ARTIFACT_TEXTURES];

Artifact::Artifact() : m_is_activated(false) {}

Artifact::~Artifact() {}

bool Artifact::init()
{
	return init({ 0.f, 0.f });
}

bool Artifact::init(vec2 position)
{
	if (!artifact_textures[0].is_valid())
	{
		if (!artifact_textures[0].load_from_file(textures_path("dungeon1/d1_wallobject_chalkboard_1-activated.png")))
		{
			fprintf(stderr, "Failed to load activated artifact texture\n");
			return false;
		}
		if (!artifact_textures[1].load_from_file(textures_path("dungeon1/d1_wallobject_chalkboard_1-deactivated.png")))
		{
			fprintf(stderr, "Failed to load deactivated artifact texture\n");
			return false;
		}
		if (!artifact_textures[2].load_from_file(textures_path("dungeon1/d1_wallobject_chalkboard_1.png")))
		{
			fprintf(stderr, "Failed to load dirty artifact texture\n");
			return false;
		}
	}

	position.y = position.y + 27;
	m_position = position;
	m_size = {static_cast<float>(artifact_textures[0].width), static_cast<float>(artifact_textures[0].height)};
	m_is_dirty = true;
  m_sound = Mix_LoadWAV(audio_path("artifact.wav"));

	// The position corresponds to the center of the texture
	float wr = artifact_textures[0].width * 0.5f;
	float hr = artifact_textures[0].height * 0.5f;

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
	m_scale.y = 0.75;

	return true;
}

void Artifact::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Artifact::draw_current(const mat3& projection, const mat3& current_transform)
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
	if (m_is_dirty)
	{
		glBindTexture(GL_TEXTURE_2D, artifact_textures[2].id);
	} 
	else if (m_is_activated)
	{
		glBindTexture(GL_TEXTURE_2D, artifact_textures[0].id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, artifact_textures[1].id);
	}
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

bool Artifact::is_activated()
{
	return m_is_activated;
}

void Artifact::set_active(bool active)
{
	Mix_PlayChannel(-1, m_sound, 0);
	m_is_activated = active;
	m_is_dirty = false;
}
