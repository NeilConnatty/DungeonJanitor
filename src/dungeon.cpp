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

	for (Room& room : m_rooms)
	{
		if (room.has_janitor_spawn_loc())
		{
			printf("%s\n", "janitor!");
			janitor_start_room = &room;
			janitor_room_position = room.get_janitor_spawn_loc();
		}

		if (room.has_hero_spawn_loc())
		{
			hero_start_room = &room;
			hero_room_position = room.get_hero_spawn_loc();
		}

		if (room.has_boss_spawn_loc())
		{
			boss_start_room = &room;
			boss_room_position = room.get_boss_spawn_loc();
		}
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

vector<Room> Dungeon::get_rooms()
{
	return m_rooms;
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
	room1.setRoomID(1);

	Room room2;
	room2.setArtifactInRoom(false);
	room2.setBossInRoom(false);
	room2.setRoomID(2);
	
	Room room3;
	room3.setArtifactInRoom(false);
	room3.setBossInRoom(true);
	room3.setRoomID(3);

	Room room4;
	room4.setArtifactInRoom(false);
	room4.setBossInRoom(false);
	room4.setRoomID(4);

	Room room5;
	room5.setArtifactInRoom(false);
	room5.setBossInRoom(false);
	room5.setRoomID(5);

	Room room6;
	room6.setArtifactInRoom(false);
	room6.setBossInRoom(false);
	room6.setRoomID(6);

	Room room7;
	room7.setArtifactInRoom(true);
	room7.setBossInRoom(false);
	room7.setRoomID(7);

	Room room8;
	room8.setArtifactInRoom(false);
	room8.setBossInRoom(false);
	room8.setRoomID(8);

	Room room9;
	room9.setArtifactInRoom(false);
	room9.setBossInRoom(false);
	room9.setRoomID(9);

	vector<Room> rooms;

	room1.setNorthRoom(nullptr);
	room1.setSouthRoom(&room4);
	room1.setEastRoom(&room2);
	room1.setWestRoom(nullptr);

	room2.setNorthRoom(nullptr);
	room2.setSouthRoom(&room5);
	room2.setEastRoom(&room3);
	room2.setWestRoom(&room1);

	
	room3.setNorthRoom(nullptr);
	room3.setSouthRoom(&room6);
	room3.setEastRoom(nullptr);
	room3.setWestRoom(&room2);

	room4.setNorthRoom(&room1);
	room4.setSouthRoom(&room7);
	room4.setEastRoom(&room5);
	room4.setWestRoom(nullptr);

	room5.setNorthRoom(&room2);
	room5.setSouthRoom(&room8);
	room5.setEastRoom(&room6);
	room5.setWestRoom(&room4);


	room6.setNorthRoom(&room3);
	room6.setSouthRoom(&room9);
	room6.setEastRoom(nullptr);
	room6.setWestRoom(&room5);

	room7.setNorthRoom(&room4);
	room7.setSouthRoom(nullptr);
	room7.setEastRoom(&room8);
	room7.setWestRoom(nullptr);

	room8.setNorthRoom(&room5);
	room8.setSouthRoom(nullptr);
	room8.setEastRoom(&room9);
	room8.setWestRoom(&room7);


	room9.setNorthRoom(&room6);
	room9.setSouthRoom(nullptr);
	room9.setEastRoom(nullptr);
	room9.setWestRoom(&room8);
	

	rooms.emplace_back(room1);
	rooms.emplace_back(room2);
	rooms.emplace_back(room3);
	rooms.emplace_back(room4);
	rooms.emplace_back(room5);
	rooms.emplace_back(room6);
	rooms.emplace_back(room7);
	rooms.emplace_back(room8);
	rooms.emplace_back(room9);

	
	ValueIteration::initialize(rooms);
	Room* target_room = ValueIteration::getNextRoom(&room4);


}

