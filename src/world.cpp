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

    /* No need for music at the moment
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

    //test all sound effects for nullptr
    if (m_background_music == nullptr)
    {
        fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
        return false;
    }

    // Playing background music indefinitely
    Mix_PlayMusic(m_background_music, -1);

    fprintf(stderr, "Loaded music");
    */

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
  
  // Make camera follow janitor
  m_camera.follow_object(&m_janitor);
  
  return true;
}

bool World::init_creatures()
{
  if (m_dungeon.janitor_start_room == nullptr || m_dungeon.hero_start_room == nullptr || m_dungeon.boss_start_room == nullptr)
  {
    fprintf(stderr, "Start rooms for dungeon not properly set.\n");
    return false;
  }

	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	m_dungeon.draw(m_camera.get_projection(w, h), m_camera.get_transform(w, h));
	vec2 janitor_position = get_world_coords_from_room_coords(m_dungeon.janitor_room_position, m_dungeon.janitor_start_room->transform, m_dungeon.transform);
  m_janitor.set_room(m_dungeon.janitor_start_room->getRoomID());
	if (!m_janitor.init(janitor_position))
	{
		fprintf(stderr, "Failed to init Janitor.\n");
		return false;
	}
	m_janitor.set_scale({ 3.f, 3.f });

	vec2 hero_position = get_world_coords_from_room_coords(m_dungeon.hero_room_position, m_dungeon.hero_start_room->transform, m_dungeon.transform);
	m_hero.setRoom(m_dungeon.hero_start_room);
	if (!m_hero.init(hero_position))
	{
		fprintf(stderr, "Failed to init Hero. \n");
		return false;

	}
	m_hero.set_scale({ 3.f, 3.f });
	m_hero.setAllRooms(&m_dungeon.get_rooms());

	vec2 boss_position = get_world_coords_from_room_coords(m_dungeon.boss_room_position, m_dungeon.boss_start_room->transform, m_dungeon.transform);
	if (!m_boss.init(boss_position))
	{
		fprintf(stderr, "Failed to init Boss. \n");
		return false;
	}
	m_boss.set_scale({ 3.f , 3.f });

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
  int w, h;
  glfwGetFramebufferSize(m_window, &w, &h);
  vec2 screen = {(float)w, (float)h};
  m_janitor.update(elapsed_ms);
  move_hero();
  m_hero.update(elapsed_ms);
  m_boss.update(elapsed_ms);
  m_dungeon.update(elapsed_ms);

  for (std::unique_ptr<Room>& room : m_dungeon.get_rooms()) {
    if (m_janitor.collides_with(room->get_door(), room->transform, m_dungeon.transform)) {
      // printf("ROOM ID: %i\n", room->getRoomID());
      if (room->getRoomID() != m_janitor.get_current_room_id()) {
        m_janitor.set_room(room->getRoomID());
      }
    }
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
  std::stringstream title_ss;
  glfwSetWindowTitle(m_window, title_ss.str().c_str());

  // Clearing backbuffer
  glViewport(0, 0, w, h);
  glDepthRange(0.00001, 10);
  const float clear_color[3] = {0.f, 0.f, 0.f};
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
  glClearDepth(1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat3 projection_2D = m_camera.get_projection(w, h);
  mat3 transform = m_camera.get_transform(w, h);

  // Drawing entities

  m_dungeon.draw(projection_2D, transform);
  m_janitor.draw(projection_2D, transform);
  m_hero.draw(projection_2D, transform);
  m_boss.draw(projection_2D, transform);

  // Presenting
  glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
    return glfwWindowShouldClose(m_window);
}


// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE JANITOR MOVEMENT HERE YO
    // key is of 'type' GLFW_KEY_
    // action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    bool move_up = false;
    bool move_down = false;
    bool move_left = false;
    bool move_right = false;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_UP){
            move_up = true;
        }
        if (key == GLFW_KEY_DOWN){
            move_down = true;
        }
        if (key == GLFW_KEY_LEFT){
            move_left = true;
        }
        if (key == GLFW_KEY_RIGHT){
            move_right = true;
        }
    }

    for (std::unique_ptr<Room>& room : m_dungeon.get_rooms()) {
        std::vector<Wall>& walls = room->get_walls();
        for (Wall& w : walls) {
            if (m_janitor.collides_with(w, room->transform, m_dungeon.transform)) {
              // printf("Wall collision\n");
                if (m_janitor.get_pos().y-get_world_coords_from_room_coords(w.get_pos(), room->transform, m_dungeon.transform).y+w.get_size().y < 15.f){
                    // printf("Collide up\n");
                    // move_up = false;
                }
                if (get_world_coords_from_room_coords(w.get_pos(), room->transform, m_dungeon.transform).y > m_janitor.get_pos().y+m_janitor.get_size().y){
                    // printf("Collide down\n");
                    // move_down = false;
                }
                if (get_world_coords_from_room_coords(w.get_pos(), room->transform, m_dungeon.transform).x+w.get_size().x < m_janitor.get_pos().x){
                    // printf("Collide left\n");
                    // move_left = false;
                }
                if (get_world_coords_from_room_coords(w.get_pos(), room->transform, m_dungeon.transform).x > m_janitor.get_pos().x+m_janitor.get_size().x){
                    // printf("Collide right\n");
                    // move_right = false;
                }
            }
        }
    }
    m_janitor.key_up(move_up);
    m_janitor.key_down(move_down);
    m_janitor.key_left(move_left);
    m_janitor.key_right(move_right);

    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE){
      for (std::unique_ptr<Room>& room : m_dungeon.get_rooms()) {
        std::vector<Puddle>& cleanables = room->get_cleanables();
        for (Puddle& p : cleanables) {
          if (p.is_enabled() && m_janitor.collides_with(p, room->transform, m_dungeon.transform)) {
            p.toggle_enable();
          }
        }
      }
    }

	// temporary keybind, probably will bind it to space once we have collisions
	if (action == GLFW_PRESS && key == GLFW_KEY_A)
	{
		m_dungeon.activate_artifact();
	}

    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R)
    {
        int w, h;
        glfwGetWindowSize(m_window, &w, &h);
        //Destructor functions for game objects go below:
    }
}

void World::move_hero()
{
	if (m_hero.get_current_room()->containsBoss())
	{
		m_hero.stop_movement();
	}
	else if (!m_hero.is_moving())
	{
		vec2 next_door_pos = get_world_coords_from_room_coords(m_hero.get_next_door_position(), m_hero.get_current_room()->transform, m_dungeon.transform);
		m_hero.set_destination(next_door_pos, Hero::destinations::DOOR);
	}
}

//left it just because.
//xpox, ypos are cursor coords.
void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
}
