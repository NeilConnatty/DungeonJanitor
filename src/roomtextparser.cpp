// roomtextparser.cpp

#include "roomtextparser.hpp"

#include <fstream>
#include <string>

// Characters
#define SPACE ' '
#define WALL 'w'
#define FLOOR 'f'
#define PUDDLE 'p'
#define ARTIFACT 'a'
#define JANITOR 'j'
#define HERO 'h'
#define BOSS 'b'
#define DOOR 'd'
#define ROOM 'r'
#define HALLWAY 'h'
#define EMPTY 'e'

//Offsets
#define ROOM_X_OFFSET 185.f
#define ROOM_Y_OFFSET 160.f;
#define WALL_X_OFFSET 10.f
#define WALL_Y_OFFSET 60.f
#define FLOOR_X_OFFSET 35.f
#define FLOOR_Y_OFFSET 25.f

const char* room_files[] = { room_path("1.rm"), room_path("2.rm"), room_path("3.rm") };
const size_t num_room_files = 3;

bool RoomParser::parseLine(std::string &line, float y, bool first_line) 
{
  float x = 0.f;
  size_t i = 0;
  wall_edge edge = NONE;
  vec2 tile_dim;

  for (; i < line.size(); ++i) 
  {
    const char &ch = line[i];

    if (ch == SPACE) 
    {
      continue;
    } 
    else if (ch == WALL) 
    {
      edge = NONE;
      if (first_line)
      {
        edge = TOP;
      }
      if (i == 0 || i == line.size() - 1)
      {
        edge = (wall_edge)(edge | VERTICAL);
      }
      wall_pairs.push_back({{x, y}, edge});
      tile_dim = Wall::get_dimensions(edge);
      x = x + tile_dim.x;
    } 
    else if (ch == FLOOR) 
    {
      floor_pos.push_back({x, y});
      tile_dim = Floor::get_dimensions();
      x = x + tile_dim.x;
    } 
	else if (ch == DOOR)
	{
		door_pos.push_back({ x,y });
		tile_dim = Floor::get_dimensions();
		x = x + tile_dim.x;
	}
    else if (ch == PUDDLE) 
    {
      puddle_pos.push_back({x, y});
      floor_pos.push_back({x, y});
      tile_dim = Floor::get_dimensions();
      x = x + tile_dim.x;
    }
    else if (ch == ARTIFACT) // At most one per room
    {
        has_artifact = true;
        artifact_pos = { x, y };
        floor_pos.push_back({ x, y });
        tile_dim = Floor::get_dimensions();
        x = x + tile_dim.x;
    }
    else if (ch == HERO) // At most one per dungeon
    {
        has_hero_spawn = true;
        hero_spawn_pos = { x, y };
        floor_pos.push_back({ x, y });
        tile_dim = Floor::get_dimensions();
        x = x + tile_dim.x;

    }
    else if (ch == BOSS) // At most one per dungeon
    {
        has_boss_spawn = true;
        boss_spawn_pos = { x, y };
        floor_pos.push_back({ x, y });
        tile_dim = Floor::get_dimensions();
        x = x + tile_dim.x;
        
    }
    else if (ch == JANITOR) // At most one per dungeon
    {
        has_janitor_spawn = true;
        janitor_spawn_pos = { x, y };
        floor_pos.push_back({ x, y });
        tile_dim = Floor::get_dimensions();
        x = x + tile_dim.x;
    }
    else 
    {
      fprintf(stderr,
              "Error parsing room file. Invalid character %c at line %d, "
              "column %d.\n",
              ch, (int)y + 1, (int)x + 1);
      return false;
    }
  }

  return true;
}

void RoomParser::clearPositions() 
{
  wall_pairs.clear();
  floor_pos.clear();
  puddle_pos.clear();
  door_pos.clear();
}

bool RoomParser::populateRoom(Room &room) 
{
    return (room.add_floors(floor_pos) && 
            room.add_walls(wall_pairs) && 
			      room.add_doors(door_pos) &&
            room.add_cleanables(puddle_pos) && 
            room.add_artifact(has_artifact, artifact_pos) &&
            room.add_hero_spawn_loc(has_hero_spawn, hero_spawn_pos) &&
            room.add_boss_spawn_loc(has_boss_spawn, boss_spawn_pos) &&
            room.add_janitor_spawn_loc(has_janitor_spawn, janitor_spawn_pos));
}

bool RoomParser::parseRoom(Room &room, const char *filename) 
{
  std::string line;
  std::ifstream file(filename);

  clearPositions();

  float y = 0.f;
  bool first_line = true;
  bool last_line = !std::getline(file, line);
  has_artifact = false;
  has_hero_spawn = false;
  has_janitor_spawn = false;
  has_boss_spawn = false;

  while (!last_line) 
  {
    if (!parseLine(line, y, first_line)) 
    {
      fprintf(stderr, "Error parsing room file: %s.\n", filename);
      return false;
    }

    if (first_line)
    {
      y = y + 60.f;
      first_line = false;
    }
    else
    {
      y = y + 25.f;
    }

    last_line = !std::getline(file, line);

    if (last_line)
    {
      for (Room::wall_pair& pair : wall_pairs)
      {
        wall_edge& edge = std::get<wall_edge>(pair);
        edge = (wall_edge)(edge | BOTTOM);
      }
    }

    if (!populateRoom(room)) 
    {
      fprintf(stderr, "Issue parsing room file: %s.\n", filename);
      return false;
    }

    clearPositions();

  }

  return true;
}

//======================= DungeonParser ======================//
bool DungeonParser::parseDungeon(std::vector<std::unique_ptr<Room>>& rooms, const char* filename)
{
  std::string line;
  std::ifstream file(filename);
  std::vector<std::string> lines;

  while (std::getline(file, line))
  {
    lines.emplace_back(line);
  }

  return parseLines(lines, rooms);
}

bool DungeonParser::parseLines(std::vector<std::string>& lines, std::vector<std::unique_ptr<Room>>& rooms)
{
  size_t row, column, num_rooms = 0;
  vec2 offset = { 0.f,0.f };
  RoomParser roomParser;

  rooms.clear();
  rooms.emplace_back(new Room);
  Room* hallway = rooms.back().get();
  if (!hallway->init())
  {
    fprintf(stderr, "Failed to init hallway room");
    return false;
  }

  for (row = 0; row < lines.size(); ++row)
  {
    std::string& line = lines[row];
    offset.x = 0.f;
    for (column = 0; column < line.size(); ++column)
    {
      char& ch = line[column];

      if (ch == ROOM)
      {
        rooms.emplace_back(new Room);
        rooms.back()->init(offset*2.f);
        roomParser.parseRoom(*rooms.back(), room_files[num_rooms%num_room_files]);
        ++num_rooms;
        // TODO: find adjacent hallway for door location -- prefer bottom
        // TODO: change room type to be classroom, office, or bathroom
      }
      else if (ch == HALLWAY)
      {
        bool topRow = (row == 0) || (lines[row-1][column] == EMPTY);
        bool bottomRow = (row == lines.size() - 1) || (lines[row + 1][column] == EMPTY);
        bool startColumn = (column == 0) || (line[column - 1] == EMPTY);
        bool endColumn = (column == line.size() - 1) || (line[column + 1] == EMPTY);
        
        addHallwayHelper(*hallway, offset, topRow, bottomRow, startColumn, endColumn);
      }
      else if (ch == EMPTY)
      {
        // do nothing! We let the offset increase
      }
      else
      {
        fprintf(stderr,
          "Error parsing room file. Invalid character %c at line %d, "
          "column %d.\n",
          ch, row + 1, column + 1);
        return false;
      }
      
      offset.x += ROOM_X_OFFSET;      
    } // for (; column < line.size(); ++column)
    offset.y += ROOM_Y_OFFSET;
  } // for (; row < lines.size(); ++row)

  // TODO: set up adjacency between rooms and hallway
  return true;
}

bool DungeonParser::addLeftTopHallway(Room& hallway, vec2 offset)
{
  return (addLeftWalls(hallway, offset, false) && addTopLeftWall(hallway, offset) &&
          addTopWalls(hallway, offset) && addFloors(hallway, offset, false) &&
          addRightFloors(hallway, offset, false));
}

bool DungeonParser::addMiddleTopHallway(Room& hallway, vec2 offset)
{
  return (addTopWalls(hallway, offset) && addFloors(hallway, offset, false) &&
          addRightFloors(hallway, offset, false));
}

bool DungeonParser::addRightTopHallway(Room& hallway, vec2 offset)
{
  return (addTopWalls(hallway, offset) && addTopRightWall(hallway, offset) &&
          addRightWalls(hallway, offset, false) && addFloors(hallway, offset, false));
}

bool DungeonParser::addLeftHallway(Room& hallway, vec2 offset)
{
  return (addLeftWalls(hallway, offset, true) && addFloors(hallway, offset, true) &&
          addRightFloors(hallway, offset, true));
}

bool DungeonParser::addMiddleHallway(Room& hallway, vec2 offset)
{
  return (addFloors(hallway, offset, true) && addRightFloors(hallway, offset, true));
}

bool DungeonParser::addRightHallway(Room& hallway, vec2 offset)
{
  return (addFloors(hallway, offset, true) && addRightWalls(hallway, offset, true));
}

bool DungeonParser::addLeftBottomHallway(Room& hallway, vec2 offset)
{
  return (addFloors(hallway, offset, true) && addBottomLeftWall(hallway, offset) &&
          addBottomWalls(hallway, offset) && addRightFloors(hallway, offset, true) &&
          addBottomRightWall(hallway, offset) && addLeftWalls(hallway, offset, true));
}

bool DungeonParser::addMiddleBottomHallway(Room& hallway, vec2 offset)
{
  return (addFloors(hallway, offset, true) && addBottomWalls(hallway, offset) &&
          addRightFloors(hallway, offset, true) &&
          addBottomRightWall(hallway, offset));
}

bool DungeonParser::addRightBottomHallway(Room& hallway, vec2 offset)
{
  return (addFloors(hallway, offset, true) && addBottomWalls(hallway, offset) &&
          addBottomRightWall(hallway, offset) &&
          addRightWalls(hallway, offset, true));
}

bool DungeonParser::addLeftTopBottomHallway(Room& hallway, vec2 offset)
{
  return (
      addLeftWalls(hallway, offset, false) && addTopLeftWall(hallway, offset) &&
      addTopWalls(hallway, offset) && addFloors(hallway, offset, false) &&
      addRightFloors(hallway, offset, false) &&
      addBottomWalls(hallway, offset) && addBottomLeftWall(hallway, offset) &&
      addBottomRightWall(hallway, offset));
}

bool DungeonParser::addRightTopBottomHallway(Room& hallway, vec2 offset)
{
  return (
    addRightWalls(hallway, offset, false) && addTopLeftWall(hallway, offset) &&
    addTopWalls(hallway, offset) && addFloors(hallway, offset, false) &&
    addRightFloors(hallway, offset, false) &&
    addBottomWalls(hallway, offset) && addBottomLeftWall(hallway, offset) &&
    addBottomRightWall(hallway, offset));
}

bool DungeonParser::addMiddleTopBottomHallway(Room& hallway, vec2 offset)
{
  return (addTopWalls(hallway, offset) && addFloors(hallway, offset, false) &&
          addRightFloors(hallway, offset, false) &&
          addBottomWalls(hallway, offset) && addBottomRightWall(hallway, offset));
}

bool DungeonParser::addLeftRightTopHallway(Room& hallway, vec2 offset)
{
  return (addLeftWalls(hallway, offset, false) &&
          addTopLeftWall(hallway, offset) && addTopWalls(hallway, offset) &&
          addFloors(hallway, offset, false) && addRightWalls(hallway, offset, false));
}

bool DungeonParser::addLeftRightBottomHallway(Room& hallway, vec2 offset)
{
  return (
      addFloors(hallway, offset, true) && addBottomLeftWall(hallway, offset) &&
      addBottomWalls(hallway, offset) && addBottomRightWall(hallway, offset) &&
      addLeftWalls(hallway, offset, true) &&
      addRightWalls(hallway, offset, true));
}

bool DungeonParser::addLeftRightMidHallway(Room& hallway, vec2 offset)
{
  return (addLeftWalls(hallway, offset, true) &&
          addFloors(hallway, offset, true) &&
          addRightWalls(hallway, offset, true));
}

bool DungeonParser::addHallwayHelper(Room &hallway,
                                     vec2 offset, bool topRow, bool bottomRow,
                                     bool startColumn, bool endColumn) 
{
  bool ret;
  if (startColumn && endColumn)
  {
    if (topRow && bottomRow)
    {
      fprintf(stderr, "Island in dungeon design text doc.\n");
      return false;
    }
    else if (topRow)
    {
      ret = addLeftRightTopHallway(hallway, offset);
    }
    else if (bottomRow)
    {
      ret = addLeftRightBottomHallway(hallway, offset);
    }
    else
    {
      ret = addLeftRightMidHallway(hallway, offset);
    }
  }
  else if (startColumn)
  {
    if (topRow && bottomRow)
    {
      ret = addLeftTopBottomHallway(hallway, offset);
    }
    else if (topRow)
    {
      ret = addLeftTopHallway(hallway, offset);
    }
    else if (bottomRow)
    {
      ret = addLeftBottomHallway(hallway, offset);
    }
    else
    {
      ret = addLeftHallway(hallway, offset);
    }
  }
  else if (endColumn)
  {
    if (topRow && bottomRow)
    {
      ret = addRightTopBottomHallway(hallway, offset);
    }
    else if (topRow)
    {
      ret = addRightTopHallway(hallway, offset);
    }
    else if (bottomRow)
    {
      ret = addRightBottomHallway(hallway, offset);
    }
    else
    {
      ret = addRightHallway(hallway, offset);
    }
  }
  else
  {
    if (topRow && bottomRow)
    {
      ret = addMiddleTopBottomHallway(hallway, offset);
    }
    else if (topRow)
    {
      ret = addMiddleTopHallway(hallway, offset);
    }
    else if (bottomRow)
    {
      ret = addMiddleBottomHallway(hallway, offset);
    }
    else
    {
      ret = addMiddleHallway(hallway, offset);
    }
  }

  return ret;
}

bool DungeonParser::addFloors(Room& hallway, vec2 offset, bool addGapFloors)
{
  size_t row, column;;
  vec2 new_offset;

  if (addGapFloors)
  {
    new_offset.y = offset.y;
    for (row = 0; row < 3; ++row)
    {
      new_offset.x = offset.x + WALL_X_OFFSET;
      for (column = 0; column < 5; ++column)
      {
        if (!hallway.add_floor(new_offset))
        {
          // TODO add print
          return false;
        }
        new_offset.x += FLOOR_X_OFFSET;
      }
      new_offset.y += FLOOR_Y_OFFSET;
    }
  }

  new_offset.y = offset.y + WALL_Y_OFFSET;
  for (row = 0; row < 4; ++row)
  {
    new_offset.x = offset.x + WALL_X_OFFSET;
    for (column = 0; column < 5; ++column)
    {
      if (!hallway.add_floor(new_offset))
      {
        // TODO add print
        return false;
      }
      new_offset.x += FLOOR_X_OFFSET;
    }
    new_offset.y += FLOOR_Y_OFFSET;
  }

  return true;
}

bool DungeonParser::addTopWalls(Room& hallway, vec2 offset)
{
  size_t column;
  vec2 new_offset = offset;
  new_offset.x += WALL_X_OFFSET;

  for (column = 0; column < 5; ++column)
  {
    if (!hallway.add_wall({ new_offset, TOP }))
    {
      return false;
    }
    new_offset.x += FLOOR_X_OFFSET;
  }

  // using 'BOTTOM' here seems unintuitive, but needed to cover up gaps
  return hallway.add_wall({ new_offset, (wall_edge)(VERTICAL | BOTTOM)});
}

bool DungeonParser::addBottomWalls(Room& hallway, vec2 offset)
{
  size_t column;
  vec2 new_offset = offset;
  new_offset.x += WALL_X_OFFSET;
  new_offset.y += (WALL_Y_OFFSET + 4.f*FLOOR_Y_OFFSET);

  for (column = 0; column < 5; ++column)
  {
    if (!hallway.add_wall({ new_offset, BOTTOM }))
    {
      return false;
    }
    new_offset.x += FLOOR_X_OFFSET;
  }

  return true;
}

bool DungeonParser::addLeftWalls(Room& hallway, vec2 offset, bool addGapFiller)
{
  size_t row;
  vec2 new_offset = offset;

  if (addGapFiller)
  {
    for (row = 0; row < 3; ++row)
    {
      if (!hallway.add_wall({ new_offset, VERTICAL }))
      {
        return false;
      }
      new_offset.y += FLOOR_Y_OFFSET;
    }
  }

  new_offset.y = offset.y + WALL_Y_OFFSET;
  for (row = 0; row < 4; ++row)
  {
    if (!hallway.add_wall({ new_offset, VERTICAL }))
    {
      return false;
    }
    new_offset.y += FLOOR_Y_OFFSET;
  }

  return true;
}

bool DungeonParser::addRightWalls(Room& hallway, vec2 offset, bool addGapFiller)
{
  size_t row;
  vec2 new_offset = offset;
  new_offset.x += (WALL_X_OFFSET + 5.f*FLOOR_X_OFFSET);

  if (addGapFiller)
  {
    for (row = 0; row < 3; ++row)
    {
      if (!hallway.add_wall({ new_offset, VERTICAL }))
      {
        return false;
      }
      new_offset.y += FLOOR_Y_OFFSET;
    }
  }

  new_offset.y = offset.y + WALL_Y_OFFSET;
  for (row = 0; row < 4; ++row)
  {
    if (!hallway.add_wall({ new_offset, VERTICAL }))
    {
      return false;
    }
    new_offset.y += FLOOR_Y_OFFSET;
  }

  return true;
}

bool DungeonParser::addTopLeftWall(Room& hallway, vec2 offset)
{
  return hallway.add_wall({ offset, (wall_edge)(VERTICAL | TOP) });
}

bool DungeonParser::addBottomLeftWall(Room& hallway, vec2 offset)
{
  vec2 new_offset = offset;
  new_offset.y += (WALL_Y_OFFSET + 4.f*FLOOR_Y_OFFSET);

  return hallway.add_wall({ new_offset, (wall_edge)(VERTICAL | BOTTOM) });
}

bool DungeonParser::addTopRightWall(Room& hallway, vec2 offset)
{
  vec2 new_offset = offset;
  new_offset.x += (WALL_X_OFFSET + 5.f*FLOOR_X_OFFSET);

  return hallway.add_wall({ new_offset, (wall_edge)(VERTICAL | TOP) });
}

bool DungeonParser::addBottomRightWall(Room& hallway, vec2 offset)
{
  vec2 new_offset = offset;
  new_offset.x += (WALL_X_OFFSET + 5.f*FLOOR_X_OFFSET);
  new_offset.y += (WALL_Y_OFFSET + 4.f*FLOOR_Y_OFFSET);

  return hallway.add_wall({ new_offset, (wall_edge)(VERTICAL | BOTTOM) });
}

bool DungeonParser::addRightFloors(Room& hallway, vec2 offset, bool addGapFiller)
{
  size_t row;
  vec2 new_offset = offset;
  new_offset.x += (WALL_X_OFFSET + 5.f*FLOOR_X_OFFSET);

  if (addGapFiller)
  {
    for (row = 0; row < 3; ++row)
    {
      if (!hallway.add_floor(new_offset)) // TODO: make way to add vertical floor tile
      {
        return false;
      }
      new_offset.y += FLOOR_Y_OFFSET;
    }
  }

  new_offset.y = offset.y + WALL_Y_OFFSET;
  for (row = 0; row < 4; ++row)
  {
    if (!hallway.add_floor(new_offset)) // TODO: make way to add vertical floor tile
    {
      return false;
    }
    new_offset.y += FLOOR_Y_OFFSET;
  }

  return true;
}
