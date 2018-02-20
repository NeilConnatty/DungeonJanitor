// dungeon.cpp

#include "dungeon.hpp"
#include "roomtextparser.hpp"
#include "ValueIteration.hpp" // testing

Dungeon::Dungeon() : 
    GameObject()
{
}

Dungeon::~Dungeon() {}

bool Dungeon::init() 
{
    m_rooms.emplace_back();
    Room& new_room = m_rooms.back();
    if (!new_room.init()) 
    {
        fprintf(stderr, "Failed to init room.\n");
        return false;
    }

    new_room.set_pos(
        {128.f, 52.f});	  // temporary values, as we don't have a real camera yet,
                          // so positions are in pixels. we will eventually have a
                          // dungeon object that contains multiple rooms.

    RoomParser parser;
  
	test_value_iteration(); // for testing

	if (!parser.parseRoom(new_room, room_path("1.rm")))
  {
    return false;
  }

    return true;
}

void Dungeon::destroy()
{
    for (Room& room : m_rooms)
    {
        room.destroy();
    }
}

void Dungeon::clean(vec2 janitor_pos)
{
    for (Room& room : m_rooms)
    {
        std::vector<Puddle>& cleanables = room.get_cleanables();
        for (Puddle& p : cleanables)
        {
            if (p.is_enabled())
            {
                // Collision stuff goes here
                if (true)
                {
                    p.toggle_enable();
                }
            }
        }
    }
}

void Dungeon::update_current(float ms)
{
}

void Dungeon::update_children(float ms)
{
    for (Room& room : m_rooms)
    {
        room.update(ms);
    }
}

void Dungeon::draw_current(const mat3& projection, const mat3& current_transform)
{
}

void Dungeon::draw_children(const mat3& projection, const mat3& current_transform)
{
    for (Room& room : m_rooms)
    {
        room.draw(projection, current_transform);
    }
}

void Dungeon::test_value_iteration()
{
	Room room1;
	room1.setArtifactInRoom(false);
	room1.setBossInRoom(false);

	Room room2;
	room2.setArtifactInRoom(true);
	room2.setBossInRoom(false);
	
	Room room3;
	room3.setArtifactInRoom(false);
	room3.setBossInRoom(true);

	vector<Room> rooms;

	room1.setWestRoom(nullptr);
	room1.setNorthRoom(nullptr);
	room1.setSouthRoom(nullptr);

	room2.setNorthRoom(nullptr);
	room2.setSouthRoom(nullptr);

	room3.setEastRoom(nullptr);
	room3.setNorthRoom(nullptr);
	room3.setSouthRoom(nullptr);


	room1.setEastRoom(&room2);
	room2.setWestRoom(&room1);
	room2.setEastRoom(&room3);
	room3.setWestRoom(&room2);

	rooms.emplace_back(room1);
	rooms.emplace_back(room2);
	rooms.emplace_back(room3);

	
	//ValueIteration::initialize(rooms);
	//ValueIteration::updateValues(rooms);


	// create dummy rooms
	// artifact
	// empty
	// boss
}
