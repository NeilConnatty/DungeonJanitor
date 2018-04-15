// Header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>

// Same as static in c, local to compilation unit
namespace
{
    namespace
    {
        void glfw_err_cb(int error, const char* desc)
        {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

World::World() 
{
    // This was previously used in the initialization list of member variables
    // for the world, which were points and salmon and fish timings. 
    // Seeding rng with random device
    // m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{

}

// World initialization
bool World::init(vec2 screen)
{
    //-------------------------------------------------------------------------
    // GLFW / OGL Initialization
    // Core Opengl 3.
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 0);
    m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "DungeonJanitor", nullptr, nullptr);
    if (m_window == nullptr)
        return false;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

                         // Load OpenGL function pointers
    gl3w_init();

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(m_window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
    glfwSetKeyCallback(m_window, key_redirect);
    glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

    //-------------------------------------------------------------------------
    // Loading music and sounds

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL Audio");
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        //fprintf(stderr, "Failed to open audio device");
        return false;
    }

    m_background_music = Mix_LoadMUS(audio_path("music.wav"));
	m_boss_fight_music = Mix_LoadMUS(audio_path("battle_in_the_winter.wav"));

    //test all sound effects for nullptr
    if (m_background_music == nullptr)
    {
        fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
        return false;
    }

	if (m_boss_fight_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
		return false;
	}
	m_boss_music_off = true;

    // Playing background music indefinitely
    Mix_PlayMusic(m_background_music, -1);
	
    fprintf(stderr, "Loaded music");
    //actually put something like return m_janitor.init();
	if (!m_dungeon.init())
	{
		fprintf(stderr, "Failed to init Dungeon.\n");
		return false;
	}

  
	if (!init_creatures())
	{
		fprintf(stderr, "Failed to init Creatures. \n");
		return false;
	}
  
  int w, h;
  glfwGetWindowSize(m_window, &w, &h);
  if (!m_camera.init(w, h))
  {
    fprintf(stderr, "failed to init Camera. \n");
    return false;
  }

  // Attach health bar to dungeon
  m_dungeon.setHealthBar(m_camera.getHealthBar());
  
	return true;
}

bool World::init_creatures()
{
  if (m_dungeon.janitor_start_room == nullptr || m_dungeon.hero_start_room == nullptr || m_dungeon.boss_start_room == nullptr)
  {
    fprintf(stderr, "Start rooms for dungeon not properly set.\n");
    return false;
  }

	m_dungeon.draw(m_camera.get_projection(), m_camera.get_transform());
	vec2 janitor_position = get_world_coords_from_room_coords(m_dungeon.janitor_room_position, m_dungeon.janitor_start_room->transform, m_dungeon.transform);
	m_janitor.set_current_room(m_dungeon.janitor_start_room);
	m_janitor.set_dungeon(&m_dungeon);
	if (!m_janitor.init(janitor_position))
	{
		fprintf(stderr, "Failed to init Janitor.\n");
		return false;
	}
	m_janitor.set_scale({ 3.f, 3.f });
	m_camera.follow_object(&m_janitor);

	vec2 boss_position = get_world_coords_from_room_coords(m_dungeon.boss_room_position, m_dungeon.boss_start_room->transform, m_dungeon.transform);
	if (!m_boss.init(boss_position))
	{
		fprintf(stderr, "Failed to init Boss. \n");
		return false;
	}
	m_dungeon.set_boss(&m_boss);
	return true;
}

bool World::init_hero()
{
	vec2 hero_position = get_world_coords_from_room_coords(m_dungeon.hero_room_position, m_dungeon.hero_start_room->transform, m_dungeon.transform);
	m_hero.setRoom(m_dungeon.hero_start_room);
	m_hero.setDungeon(&m_dungeon);

	if (!m_hero.init(hero_position))
	{
		fprintf(stderr, "Failed to init Hero. \n");
		return false;

	}

	m_hero.setAllRooms(&m_dungeon.get_rooms());
	m_dungeon.spawn_hero();
	return true;

}

// Releases all the associated resources
void World::destroy()
{
	
  if (m_background_music != nullptr)
    Mix_FreeMusic(m_background_music);

  Mix_CloseAudio();
  SDL_Quit();

  m_dungeon.destroy();
  m_janitor.destroy();
  m_hero.destroy();
  m_boss.destroy();
  // Destructors for game objects here
  glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
  if (!game_is_over)
  {
	  m_janitor.update(elapsed_ms);
	  m_boss.update(elapsed_ms);
	  m_dungeon.update(elapsed_ms);
	  m_camera.update(elapsed_ms);
	  m_janitor.check_movement();
	  if (m_dungeon.hero_has_spawned())
	  {
		  m_hero.update(elapsed_ms);
	  }
	  else if (m_dungeon.should_spawn_hero())
	  {
		  if (!init_hero())
		  {
			  return false;
		  }
	  }
	  if (m_dungeon.has_boss_fight_started())
	  {
		  if (m_boss_music_off)
		  {
			  Mix_PlayMusic(m_boss_fight_music, -1);
			  m_boss_music_off = false;
		  }
		  if (m_dungeon.get_boss_fight_dungeon_health() == 0.0f)
		  {
			  game_over();
		  }
		  else
		  {
			  m_dungeon.boss_start_room->spawn_debris();

		  }
	  }

  }
  else 
  {
	  m_camera.update(elapsed_ms);
	  m_game_over_screen.update(elapsed_ms);
  }
  return true;
}

// Render our game world
void World::draw()
{
  // Clearing error buffer
  gl_flush_errors();

  // Getting size of window
  int w, h;
  glfwGetFramebufferSize(m_window, &w, &h);

  // Updating window title with points
  if (!m_dungeon.should_spawn_hero()) {
	  std::stringstream title_ss;
	  title_ss << "Hero Arrival In: " << m_dungeon.get_hero_timer();
	  glfwSetWindowTitle(m_window, title_ss.str().c_str());
  } 
  else if (m_dungeon.has_boss_fight_started())
  {
	  std::stringstream title_ss;
	  title_ss << "Work Day Ends In: " << m_dungeon.get_boss_fight_timer();
	  glfwSetWindowTitle(m_window, title_ss.str().c_str());
  }

  // Clearing backbuffer
  glViewport(0, 0, w, h);
  glDepthRange(0.00001, 10);
  const float clear_color[3] = {0.f, 0.f, 0.f};
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
  glClearDepth(1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat3 projection_2D = m_camera.get_projection();
  mat3 transform = m_camera.get_transform();

  // Drawing entities
  if (!game_is_over) 
  {
	  m_dungeon.draw(projection_2D, transform);
	  m_janitor.draw(projection_2D, transform);
	  m_boss.draw(projection_2D, transform);
    m_camera.draw(projection_2D, transform);
    if (m_dungeon.hero_has_spawned())
    {
      m_hero.draw(projection_2D, transform);
    }
  }
  else
  {
	  m_game_over_screen.draw(projection_2D, transform);
  }
  // Presenting
  glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
    return glfwWindowShouldClose(m_window);
}

void World::game_over() 
{
	game_is_over = true;
	m_game_over_screen.init(m_janitor.get_pos());
	m_camera.follow_object(&m_game_over_screen);
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE JANITOR MOVEMENT HERE YO
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_UP) {
			m_janitor.key_up(true);
		}
		if (key == GLFW_KEY_DOWN) {
			m_janitor.key_down(true);
		}
		if (key == GLFW_KEY_LEFT) {
			m_janitor.key_left(true);
		}
		if (key == GLFW_KEY_RIGHT) {
			m_janitor.key_right(true);
		}
	}
    if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_UP){
        m_janitor.key_up(false);
      }
      if (key == GLFW_KEY_DOWN){
        m_janitor.key_down(false);
      }
      if (key == GLFW_KEY_LEFT){
        m_janitor.key_left(false);
      }
      if (key == GLFW_KEY_RIGHT){
        m_janitor.key_right(false);
      }
    }
    
    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		for (unique_ptr<Room>& r : m_dungeon.get_rooms())
		{
			Room* room = r.get();
			r->clean(&m_janitor, m_dungeon.transform);
		}
    }
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R && game_is_over)
	{
		game_is_over = false;
		//Destructor functions for game objects go below:
		m_dungeon.destroy();
		m_janitor.destroy();
		m_hero.destroy();
		m_boss.destroy();
		m_camera.destroy();
		
		
		if (!m_dungeon.init())
		{
			fprintf(stderr, "Failed to reinit Dungeon.\n");
			return;
		}
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
		if (!m_camera.init(w, h))
		{
			fprintf(stderr, "failed to reinit Camera. \n");
			return;
		}
		m_dungeon.setHealthBar(m_camera.getHealthBar());
		if (!init_creatures())
		{
			fprintf(stderr, "Failed to reinit Creatures. \n");
			return;
		}
	}
}

//left it just because.
//xpox, ypos are cursor coords.
void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
}
