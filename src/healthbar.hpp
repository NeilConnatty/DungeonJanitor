#pragma once

#include "common.hpp"
#include "gameobject.hpp"

//#include <vector>
//#include <array>
//#include <memory>

using namespace std;

class HealthBar : public GameObject
{

	static Texture m_health_texture, m_bar_texture;

private:
	class Health : public GameObject
	{
	public:

		// Creates all the associated render resources and default transform
		bool init() { return false; }
		bool init(vec2 window_size);

		// Releases all associated resources
		void destroy();

		void draw_current(const mat3& projection, const mat3& current_transform)override;
		void draw_children(const mat3& projection, const mat3& current_transform)override {};
		void update_current(float ms)override;
		void update_children(float ms)override {};

		bool validate_textures();
		float m_percent_filled;

	};
	class Bar : public GameObject
	{
	public:
		// Creates all the associated render resources and default transform
		bool init() { return false; }
		bool init(vec2 window_size);

		// Releases all associated resources
		void destroy();

		void draw_current(const mat3& projection, const mat3& current_transform)override;
		void draw_children(const mat3& projection, const mat3& current_transform)override {};
		void update_current(float ms)override;
		void update_children(float ms)override {};

		bool validate_textures();
		
		
	};

public:
	HealthBar() {};
	~HealthBar() {};

	// Creates all the associated render resources and default transform
	bool init() { return false; }
	bool init(vec2 window_size);

	// Releases all associated resources
	void destroy();

	static Texture* get_health_texture() { return &m_health_texture; }
	static Texture* get_bar_texture() { return &m_bar_texture; }

	void draw_current(const mat3& projection, const mat3& current_transform)override {};
	void draw_children(const mat3& projection, const mat3& current_transform)override;
	void update_current(float ms)override;
	void update_children(float ms)override;
	void set_percent_filled(float percent_filled);

private:
	float m_percent_filled;
	Health m_health;
	Bar m_bar;
};
