#pragma once

#include "common.hpp"
#include "room.hpp"
#include "door.hpp"
#include "dungeon.hpp"
#include "Cleanable/cleanable.hpp"

using namespace std;

enum location
{
  NORTH,
  EAST,
  SOUTH,
  WEST
};

struct door_info;
using room_id = int;
using room_pair = std::pair<room_id, location>;

class RoomParser
{
public:
	bool parseRoom(Room& room, const char* filename);
  vector<door_info>& get_door_info() { return door_infos; }

private:
	void clearPositions();
	bool parseLine(string& line, float y, bool first_line, bool last_line);
	bool populateRoom(Room &room);

private:

	bool has_artifact;
	bool has_boss_spawn;
	bool has_hero_spawn;
	bool has_janitor_spawn;

	vec2 artifact_pos;
	vec2 boss_spawn_pos;
	vec2 hero_spawn_pos;
	vec2 janitor_spawn_pos;
	vector<pair<Cleanable::types, vec2>> cleanable_pos;
	
	vector<vec2> floor_pos;
	vector<door_info> door_infos;
	vector<Room::wall_pair> wall_pairs;

  room_type room_t;
};

class DungeonParser
{
public:
  DungeonParser();

  bool parseDungeon(std::vector<std::unique_ptr<Room>>& rooms, const char* filename, Dungeon& dungeon);

private:
  bool parseLines(std::vector<std::string>& lines, std::vector<std::unique_ptr<Room>>& rooms, Dungeon& dungeon);
  bool addLeftTopHallway(Room& hallway, vec2 offset);
  bool addMiddleTopHallway(Room& hallway, vec2 offset);
  bool addRightTopHallway(Room& hallway, vec2 offset);
  bool addLeftHallway(Room& hallway, vec2 offset);
  bool addMiddleHallway(Room& hallway, vec2 offset);
  bool addRightHallway(Room& hallway, vec2 offset);
  bool addLeftBottomHallway(Room& hallway, vec2 offset);
  bool addMiddleBottomHallway(Room& hallway, vec2 offset);
  bool addRightBottomHallway(Room& hallway, vec2 offset); 
  bool addLeftTopBottomHallway(Room& hallway, vec2 offset);
  bool addRightTopBottomHallway(Room& hallway, vec2 offset);
  bool addMiddleTopBottomHallway(Room& hallway, vec2 offset);
  bool addLeftRightTopHallway(Room& hallway, vec2 offset);
  bool addLeftRightMidHallway(Room& hallway, vec2 offset);
  bool addLeftRightBottomHallway(Room& hallway, vec2 offset);
  bool addTopWalls(Room& hallway, vec2 offset);
  bool addBottomWalls(Room& hallway, vec2 offset);
  bool addLeftWalls(Room& hallway, vec2 offset, bool addGapFiller);
  bool addRightWalls(Room& hallway, vec2 offset, bool addGapFiller);
  bool addTopLeftWall(Room& hallway, vec2 offset);
  bool addTopRightWall(Room& hallway, vec2 offset);
  bool addBottomLeftWall(Room& hallway, vec2 offset);
  bool addBottomRightWall(Room& hallway, vec2 offset);
  bool addFloors(Room& hallway, vec2 offset, bool addGapFloors);
  bool addRightFloors(Room& hallway, vec2 offset, bool addGapFiller);
  bool addHallwayHelper(Room &hallway, vec2 offset,
                        bool topRow, bool bottomRow, bool startColumn,
                        bool endColumn);
  bool buildRooms(std::vector<std::string>& lines, std::vector<std::unique_ptr<Room>>& rooms, Dungeon& dungeon, Room& hallway);
  bool buildHallway(std::vector<std::string>& lines, std::vector<std::unique_ptr<Room>>& rooms, Dungeon& dungeon, Room& hallway);
  bool validRoomID(int room_id);
  bool addAdjacency(std::unordered_map<room_id, std::vector<room_pair>>& room_adjacency_map, char& adj_ch, room_id id, location loc);


private:
  std::vector<std::string> m_room_files;
};

