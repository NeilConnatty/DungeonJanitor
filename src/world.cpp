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
  
  return true;
}

bool World::init_creatures()
{
	Room* janitor_start_room;
	Room* hero_start_room;
	Room* boss_start_room;
	vec2 janitor_room_position;
	vec2 hero_room_position;
	vec2 boss_room_position;
	for (Room& room : m_dungeon.get_rooms())
	{
		if (room.has_janitor_spawn_loc())
		{
			printf("%s\n", "janitor!");
			janitor_start_room = &room;
			janitor_room_position = room.get_janitor_spawn_loc();
		}

		if (room.has_hero_spawn_loc())
		{
			printf("%s\n", "hero!");
			hero_start_room = &room;
			hero_room_position = room.get_hero_spawn_loc();
		}

		if (room.has_boss_spawn_loc())
		{
			boss_start_room = &room;
			boss_room_position = room.get_boss_spawn_loc();
		}
	}
	vec3 janitor_room_position_3d = { janitor_room_position.x, janitor_room_position.y, 1 };
	printf("%f", janitor_room_position_3d.x);
	printf("%f\n", janitor_room_position_3d.y);
	vec3 janitor_dungeon_position = mult(janitor_start_room->transform, janitor_room_position_3d);
	printf("%f", janitor_dungeon_position.x);
	printf("%f\n", janitor_dungeon_position.y);
	vec3 janitor_world_position = mult(m_dungeon.transform, janitor_dungeon_position);
	printf("%f", janitor_world_position.x);
	printf("%f\n", janitor_world_position.y);
	vec2 janitor_position = { janitor_world_position.x, janitor_world_position.y };
	printf("%f", janitor_position.x);
	printf("%f\n", janitor_position.y);
	if (!m_janitor.init(janitor_position))
	{
		fprintf(stderr, "Failed to init Janitor.\n");
		return false;
	}
	m_janitor.set_scale({ 3.f, 3.f });

	vec2 hero_position = hero_room_position;
	printf("%f", hero_position.x);
	printf("%f\n", hero_position.y);
	if (!m_hero.init(hero_position))
	{
		fprintf(stderr, "Failed to init Hero. \n");
		return false;

	}
	m_hero.set_scale({ 3.f, 3.f });

	vec2 boss_position = boss_room_position;
	printf("%f", boss_position.x);
	printf("%f\n", boss_position.y);
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
    //Destructors for game objects here
    glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    vec2 screen = { (float)w, (float)h };
    m_janitor.update(elapsed_ms);
	m_hero.set_destination(m_janitor.get_pos());
	m_hero.update(elapsed_ms);
	m_boss.update(elapsed_ms);
    m_dungeon.update(elapsed_ms);

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
    const float clear_color[3] = { 0.f, 0.f, 0.f };
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)w;// *0.5;
    float bottom = (float)h;// *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
    mat3 identity_transform{ {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f} };

    // Drawing entities
    m_dungeon.draw(projection_2D, identity_transform);
    m_janitor.draw(projection_2D, identity_transform);
	m_hero.draw(projection_2D, identity_transform);
	m_boss.draw(projection_2D, identity_transform);
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
    
    //flip a boolean on press and release of the following keys for movement!
    if ((action == GLFW_PRESS || action == GLFW_RELEASE) && key == GLFW_KEY_UP)
    {
        m_janitor.key_up();
    }
    if ((action == GLFW_PRESS || action == GLFW_RELEASE) && key == GLFW_KEY_DOWN)
    {
        m_janitor.key_down();
    }
    if ((action == GLFW_PRESS || action == GLFW_RELEASE) && key == GLFW_KEY_LEFT)
    {
        m_janitor.key_left();
    }
    if ((action == GLFW_PRESS || action == GLFW_RELEASE) && key == GLFW_KEY_RIGHT)
    {
        m_janitor.key_right();
    }

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
	{
		m_dungeon.clean(m_janitor.get_pos());
	}

    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R)
    {
        int w, h;
        glfwGetWindowSize(m_window, &w, &h);
        //Destructor functions for game objects go below:
    }
}

//left it just because.
//xpox, ypos are cursor coords.
void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
}
