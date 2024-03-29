#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

// stlib
#include <vector>
#include <sstream>
#include <iostream>
void gl_flush_errors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
	}

	return true;
}

float dot(vec2 l, vec2 r)
{
	return l.x * r.x + l.y * r.y;
}

float dot(vec3 l, vec3 r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

mat3 mul(const mat3 & l, const mat3 & r)
{
	mat3 l_t = { { l.c0.x, l.c1.x, l.c2.x},
	{ l.c0.y, l.c1.y, l.c2.y } ,
	{ l.c0.z, l.c1.z, l.c2.z } };

	mat3 ret;
	ret.c0.x = dot(l_t.c0, r.c0);
	ret.c0.y = dot(l_t.c1, r.c0);
	ret.c0.z = dot(l_t.c2, r.c0);

	ret.c1.x = dot(l_t.c0, r.c1);
	ret.c1.y = dot(l_t.c1, r.c1);
	ret.c1.z = dot(l_t.c2, r.c1);

	ret.c2.x = dot(l_t.c0, r.c2);
	ret.c2.y = dot(l_t.c1, r.c2);
	ret.c2.z = dot(l_t.c2, r.c2);
	return ret;
}


//  not giving me the right result in world.cpp
vec3 mult(const mat3 & l, const vec3 & r)
{
	mat3 l_t = { { l.c0.x, l.c1.x, l.c2.x },
	{ l.c0.y, l.c1.y, l.c2.y } ,
	{ l.c0.z, l.c1.z, l.c2.z } };

	vec3 ret;
	ret.x = dot(l_t.c0, r);
	ret.y = dot(l_t.c1, r);
	ret.z = dot(l_t.c2, r);
	return ret;
}

mat3 operator*(const mat3 & l, const mat3 & r)
{
	return mul(l, r);
}

vec2 operator*(const vec2& vec, const float& n)
{
    return {vec.x * n, vec.y * n};
}

vec3 operator*(const vec3& vec, const float& n)
{
  return { vec.x * n, vec.y * n, vec.z*n };
}

mat3 operator*(const mat3& m, const float& f)
{
  return { m.c0 * f, m.c1 * f, m.c2 * f };
}

vec2 normalize(vec2 v)
{
	float m = sqrtf(dot(v, v));
	return { v.x / m, v.y / m };
}

mat3 mat_of_cofactors(mat3 m)
{
  vec3 c0, c1, c2;
  c0 = { m.c0.x,      -1 * m.c0.y,      m.c0.z };
  c1 = { -1 * m.c1.x,      m.c1.y, -1 * m.c1.z };
  c2 = { m.c2.x,      -1 * m.c2.y,      m.c2.z };

  return { c0, c1, c2 };
}

mat3 adjucate(mat3 m)
{
  vec3 c0, c1, c2;
  c0 = { m.c0.x, m.c1.x, m.c2.x };
  c1 = { m.c0.y, m.c1.y, m.c2.y };
  c2 = { m.c0.z, m.c1.z, m.c2.z };

  return { c0, c1, c2 };
}

mat3 mat_of_minor(mat3 m)
{
  vec3 c0, c1, c2;
  
  c0.x = m.c1.y * m.c2.z - m.c1.z * m.c2.y;
  c0.y = m.c1.x * m.c2.z - m.c1.z * m.c2.x;
  c0.z = m.c1.x * m.c2.y - m.c1.y * m.c2.x;

  c1.x = m.c0.y * m.c2.z - m.c0.z * m.c2.y;
  c1.y = m.c0.x * m.c2.z - m.c0.z * m.c2.x;
  c1.z = m.c0.x * m.c2.y - m.c0.y * m.c2.x;

  c2.x = m.c0.y * m.c1.z - m.c0.z * m.c1.y;
  c2.y = m.c0.x * m.c1.z - m.c0.z * m.c1.x;
  c2.z = m.c0.x * m.c1.y - m.c0.y * m.c1.x;

  return { c0, c1, c2 };
}

float det(mat3 m, mat3 minors) 
{
  return m.c0.x * minors.c0.x - m.c1.x * minors.c1.x + m.c1.z * minors.c1.z;
}

float det(mat3 m)
{
  mat3 minors = mat_of_minor(m);
  return det(m, minors);
}

std::string get_timer_string(float timer)
{
	double minutesRemainder = (timer) / 60000;
	int minutes = minutesRemainder;
	double secondsRemainder = (minutesRemainder - minutes) * 60;
	int seconds = secondsRemainder;
	std::string minutes_str = std::to_string(minutes);
	std::string seconds_str = std::to_string(seconds);

	if (minutes <= 0 && seconds < 0)
	{
		return "0:00";
	}
	if (seconds < 10)
	{
		seconds_str = "0" + seconds_str;
	}
	return  minutes_str + ":" + seconds_str;
}


vec2 get_world_coords_from_room_coords(vec2 room_coords, mat3 room_transform, mat3 dungeon_transform)
{

	vec3 room_position_3d = { room_coords.x, room_coords.y, 1.0 };
	vec3 dungeon_position = mult(room_transform, room_position_3d);
	vec3 world_position = mult(dungeon_transform, dungeon_position);
	return { world_position.x, world_position.y };
}

mat3 inverse(mat3 m)
{
  // The lovely process for calculating the inverse was taken from here:
  // https://www.mathsisfun.com/algebra/matrix-inverse-minors-cofactors-adjugate.html

  mat3 minors = mat_of_minor(m);
  mat3 cofactors = mat_of_cofactors(minors);
  mat3 adj = adjucate(cofactors);

  float detoverone = 1 / det(m, minors);

  return adj * detoverone;
}

Texture::Texture()
{

}

Texture::~Texture()
{
	if (id != 0) glDeleteTextures(1, &id);
}

bool Texture::load_from_file(const char* path)
{
	if (path == nullptr)
		return false;
	
	stbi_uc* data = stbi_load(path, &width, &height, NULL, 4);
	if (data == NULL) {
		//next line just gets a bit more failure information
		std::cout << stbi_failure_reason();
		fprintf(stderr, "data is null");
		return false;
	}
		

	gl_flush_errors();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);
	return !gl_has_errors();
}

bool Texture::is_valid()const
{
	return id != 0;
}

namespace
{
	bool gl_compile_shader(GLuint shader)
	{
		glCompileShader(shader);
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint log_len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetShaderInfoLog(shader, log_len, &log_len, log.data());
			glDeleteShader(shader);

			fprintf(stderr, "GLSL: %s", log.data());
			return false;
		}

		return true;
	}
}

bool Effect::load_from_file(const char* vs_path, const char* fs_path)
{
	gl_flush_errors();

	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);

	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path, fs_path);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);

	// Compiling
	// Shaders already delete if compilation fails
	if (!gl_compile_shader(vertex))
		return false;

	if (!gl_compile_shader(fragment))
	{
		glDeleteShader(vertex);
		return false;
	}

	// Linking
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	{
		GLint is_linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(program, log_len, &log_len, log.data());

			release();
			fprintf(stderr, "Link error: %s", log.data());
			return false;
		}
	}

	if (gl_has_errors())
	{
		release();
		fprintf(stderr, "OpenGL errors occured while compiling Effect");
		return false;
	}

	return true;
}

void Effect::release()
{
	glDeleteProgram(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Renderable::transform_begin()
{
	transform = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} };
}

void Renderable::transform_scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	transform = mul(transform, S);
}

void Renderable::transform_rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	transform = mul(transform, R);
}

void Renderable::transform_translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	transform = mul(transform, T);
}

void Renderable::transform_end()
{
	//
}
