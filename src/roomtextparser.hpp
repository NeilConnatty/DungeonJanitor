#pragma once

#include "common.hpp"
#include "room.hpp"
#include "dungeon.hpp"
#include "Cleanable/cleanable.hpp"

using namespace std;

class RoomParser
{
public:
	bool parseRoom(Room& room, const char* filename);
  vector<vec2>& get_door_pos() { return door_pos; }

private:
	void clearPositions();
	bool parseLine(string& line, float y, bool first_line);
	bool populateRoomExceptWalls(Room &room);
	bool populateRoomWalls(Room &room);

private:

	bool has_artifact;
	bool has_boss_spawn;
	bool has_hero_spawn;
	bool has_janitor_spawn;

	vec2 artifact_pos;
	vec2 boss_spawn_pos;
	vec2 hero_spawn_pos;
	vec2 janitor_spawn_pos;
	
	vector<vec2> floor_pos;
	vector<pair<Cleanable::types, vec2>> cleanable_pos;
	vector<vec2> door_pos;
	vector<Room::wall_pair> wall_pairs;  
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

private:
  std::vector<std::string> m_room_files;
};

