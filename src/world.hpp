#pragma once

// internal
#include "common.hpp"
#include "dungeon.hpp"
#include "janitor.hpp"
#include "hero.hpp"
#include "boss.hpp"
#include "camera.hpp"
#include "gameover.hpp"
#include "winscreen.hpp"
#include "ValueIteration.hpp"
// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	bool init_creatures();
	bool init_hero();

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ? (closes the window)
	bool is_over()const;

	//sets game_is_over = true for drawing the game over screen
	void game_over(bool didWin);

private:


	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// Window handle
	GLFWwindow* m_window;

	// Dungeon
	Dungeon m_dungeon;

	// Game entities
	Mix_Music* m_background_music;

	//Janitor
	Janitor m_janitor;


	// Hero
	Hero m_hero;

	// Boss
	Boss m_boss;

	//Camera
	Camera m_camera;

	//Game Over Screen
	GameOver m_game_over_screen;
	Winscreen m_win_screen;
	bool m_did_win;
	bool game_is_over;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
