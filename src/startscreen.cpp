#include "StartScreen.hpp"
#include <algorithm>
Texture StartScreen::startscreen1;
Texture StartScreen::startscreen2;

StartScreen::StartScreen() {}
StartScreen::~StartScreen() {}

void StartScreen::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

bool StartScreen::init() { return false; }

//Always call this with m_janitor.get_pos() as the input
bool StartScreen::init(vec2 pos) {

	if (!startscreen1.is_valid())
	{
		if (!startscreen1.load_from_file(textures_path("/ui/start_1.png")))
		{
			fprintf(stderr, "Failed to load StartScreen texture1\n");
			return false;
		}
	}
	if (!startscreen2.is_valid())
	{
		if (!startscreen2.load_from_file(textures_path("/ui/start_2.png")))
		{
			fprintf(stderr, "Failed to load StartScreen texture2\n");
			return false;
		}
	}

	//scale of the texture and resulting mesh
	//all the player textures are the same size so it's fine to pick one arbitrarily
	float wr = startscreen1.width * 0.5f;
	float hr = startscreen1.height * 0.5f;

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

	m_animation_time = 0;
	m_curr_tex = &startscreen1;
	m_position = pos;
	return true;
}
void StartScreen::draw_current(const mat3& projection, const mat3& current_transform) {
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
	//dereferencing a null pointer ic ic.
	glBindTexture(GL_TEXTURE_2D, m_curr_tex->id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&current_transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
void StartScreen::draw_children(const mat3& projection, const mat3& current_transform) {

}
void StartScreen::update_current(float ms) {
	const int NUM_FRAMES = 2;
	const int FRAME_TIMING = 800; //1000ms per frame
	m_animation_time += ms;
	m_animation_time = (int)m_animation_time % (NUM_FRAMES*FRAME_TIMING);
	if (m_animation_time < FRAME_TIMING) m_curr_tex = &startscreen1;
	else m_curr_tex = &startscreen2;
	if (m_animation_time > NUM_FRAMES*FRAME_TIMING) m_animation_time = 0;
}
void StartScreen::update_children(float ms) {//stub
}

