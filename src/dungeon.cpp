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

test_value_iteration(); // for testing  

RoomParser parser;

	/**************3rd Room ****************/
	m_rooms.emplace_back(std::make_unique<Room>());
	Room* new_room_3 = m_rooms.back().get();
	if (!new_room_3->init())
	{
		fprintf(stderr, "Failed to init room3.\n");
		return false;
	}

	new_room_3->set_pos({ 128.f, -1170.f }); // temporary values, eventually we will want to have
											// a parser that creates the dungeon layouts
	if (!parser.parseRoom(*new_room_3, room_path("3.rm")))
	{
		return false;
	}
	  /**************2nd Room ****************/
	  m_rooms.emplace_back(std::make_unique<Room>());
	  Room* new_room_2 = m_rooms.back().get();
	  if (!new_room_2->init())
	  {
		fprintf(stderr, "Failed to init room.\n");
		return false;
	  }

	  new_room_2->set_pos({ 128.f, -550.f }); // temporary values, eventually we will want to have
											 // a parser that creates the dungeon layouts
	  if (!parser.parseRoom(*new_room_2, room_path("2.rm")))
	  {
		return false;
	  }

	  /**************1st Room ****************/

	  m_rooms.emplace_back(std::make_unique<Room>());
		Room* new_room = m_rooms.back().get();
		if (!new_room->init()) 
		{
			fprintf(stderr, "Failed to init room.\n");
			return false;
		}

	  new_room->set_pos(
		  {128.f, 70.f}); // temporary values, eventually we will want to have
						  // a parser that creates the dungeon layouts

	  if (!parser.parseRoom(*new_room, room_path("1.rm")))
	  {
		return false;
	  }


    for (unique_ptr<Room>& room : m_rooms)
    {
        if (room->has_janitor_spawn_loc())
        {	
            janitor_start_room = room.get();
            janitor_room_position = room->get_janitor_spawn_loc();
        }

        if (room->has_hero_spawn_loc())
        {
            hero_start_room = room.get();
            hero_room_position = room->get_hero_spawn_loc();
        }

        if (room->has_boss_spawn_loc())
        {
            boss_start_room = room.get();
            boss_room_position = room->get_boss_spawn_loc();
        }
    }
  
  Door& door = new_room->get_m_doors()->front();
  door.set_pos({ 220.0, 18.0 }); // temp value
  door.toggle_enable(); // temp hack
  new_room->setRoomID(1);
  new_room->set_north_room(new_room_2, &door);
  new_room_2->setRoomID(2);
  new_room_2->set_south_room(new_room, &door);
  Door& door2 = new_room_3->get_m_doors()->front();
  door2.set_pos({ 220.0, -20.0}); //temp value
  door2.toggle_enable(); // temp hack
  new_room_2->set_north_room(new_room_3, &door2);
  new_room_3->set_south_room(new_room_2, &door2);
  new_room_3->setRoomID(3);

 
  return true;
}

void Dungeon::destroy()
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->destroy();
	}
}

vector<unique_ptr<Room>>& Dungeon::get_rooms()
{
	return m_rooms;
}

void Dungeon::clean()
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		std::vector<Puddle>& cleanables = room->get_cleanables();
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

void Dungeon::activate_artifact()
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		Room* room_ptr = room.get();
		if (room_ptr->containsArtifact())
		{
			room_ptr->get_artifact()->set_active(true);
		}
	}
}

void Dungeon::update_current(float ms)
{
}

void Dungeon::update_children(float ms)
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->update(ms);
	}
}

void Dungeon::draw_current(const mat3& projection, const mat3& current_transform)
{
}

void Dungeon::draw_children(const mat3& projection, const mat3& current_transform)
{
	for (std::unique_ptr<Room>& room : m_rooms)
	{
		room->draw(projection, current_transform);
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

	Door door1;
	Door door2;
    room1.set_north_room(nullptr, nullptr);
    room1.set_south_room(&room4, &door1);
    room1.set_east_room(&room2, &door2);
    room1.set_west_room(nullptr, nullptr);

	Door door3;
	Door door4;
    room2.set_north_room(nullptr, nullptr);
    room2.set_south_room(&room5, &door3);
    room2.set_east_room(&room3, &door4);
    room2.set_west_room(&room1, &door2);

	Door door5;
    room3.set_north_room(nullptr, nullptr);
    room3.set_south_room(&room6, &door5);
    room3.set_east_room(nullptr, nullptr);
    room3.set_west_room(&room2, &door4);

	Door door6;
	Door door7;
    room4.set_north_room(&room1, &door1);
    room4.set_south_room(&room7, &door6);
    room4.set_east_room(&room5,  &door7);
    room4.set_west_room(nullptr, nullptr);

	Door door8;
	Door door9;
    room5.set_north_room(&room2, &door3);
    room5.set_south_room(&room8, &door8);
    room5.set_east_room(&room6, &door9);
    room5.set_west_room(&room4, &door7);

	Door door10;
    room6.set_north_room(&room3, &door5);
    room6.set_south_room(&room9, &door10);
    room6.set_east_room(nullptr, nullptr);
    room6.set_west_room(&room5, &door9);

	Door door11;
    room7.set_north_room(&room4, &door6);
    room7.set_south_room(nullptr, nullptr);
    room7.set_east_room(&room8, &door11);
    room7.set_west_room(nullptr, nullptr);

	Door door12;
    room8.set_north_room(&room5, &door8);
    room8.set_south_room(nullptr, nullptr);
    room8.set_east_room(&room9, &door12);
    room8.set_west_room(&room7, &door11);


    room9.set_north_room(&room6, &door10);
    room9.set_south_room(nullptr, nullptr);
    room9.set_east_room(nullptr,nullptr);
    room9.set_west_room(&room8, &door12);
    

    rooms.emplace_back(room1);
    rooms.emplace_back(room2);
    rooms.emplace_back(room3);
    rooms.emplace_back(room4);
    rooms.emplace_back(room5);
    rooms.emplace_back(room6);
    rooms.emplace_back(room7);
    rooms.emplace_back(room8);
    rooms.emplace_back(room9);

    //Room* target_room = ValueIteration::getNextRoom(&room4); //Jay commented out after VI returned room direction
}
