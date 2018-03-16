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
#define HALLWAY 'h'
#define EMPTY 'e'
#define ROOM 'r'

//Offsets
#define ROOM_X_OFFSET 185.f
#define ROOM_Y_OFFSET 160.f;
#define WALL_X_OFFSET 10.f
#define WALL_Y_OFFSET 60.f
#define FLOOR_X_OFFSET 35.f
#define FLOOR_Y_OFFSET 25.f

bool RoomParser::parseLine(std::string &line, float y, bool first_line, bool last_line) 
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
      floor_pos.push_back({ x, y });
      tile_dim = Floor::get_dimensions();
      x = x + tile_dim.x;
    }
    else if (ch == DOOR)
    {
      if (first_line || last_line)
      {
        door_pairs.push_back(std::make_pair<vec2, Door::door_dir>({ x,y }, Door::HORIZONTAL));
      }
      else
      {
        door_pairs.push_back(std::make_pair<vec2, Door::door_dir>({ x,y }, Door::VERTICAL));
      }

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
  door_pairs.clear();
}

bool RoomParser::populateRoomWalls(Room &room)
{
  return room.add_walls(wall_pairs);
}

bool RoomParser::populateRoomExceptWalls(Room &room) 
{
  return (room.add_floors(floor_pos) && room.add_cleanables(puddle_pos) &&
          room.add_artifact(has_artifact, artifact_pos) &&
          room.add_hero_spawn_loc(has_hero_spawn, hero_spawn_pos) &&
          room.add_boss_spawn_loc(has_boss_spawn, boss_spawn_pos) &&
          room.add_janitor_spawn_loc(has_janitor_spawn, janitor_spawn_pos));
}

bool RoomParser::parseRoom(Room &room, const char *filename) 
{
  std::string line;
  std::ifstream file(filename);
  std::vector<std::string> lines;

  clearPositions();

  while (std::getline(file, line))
  {
    lines.emplace_back(line);
  }

  float y = 0.f;
  bool first_line = true;
  bool last_line = false;
  has_artifact = false;
  has_hero_spawn = false;
  has_janitor_spawn = false;
  has_boss_spawn = false;

  for (size_t i = 0; i < lines.size(); ++i)
  {
    if (i == (lines.size() - 1))
    {
      last_line = true;
    }

    if (!parseLine(lines[i], y, first_line, last_line)) 
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

    if (last_line)
    {
      for (Room::wall_pair& pair : wall_pairs)
      {
        wall_edge& edge = std::get<wall_edge>(pair);
        edge = (wall_edge)(edge | BOTTOM);
      }
    }

    if (!populateRoomWalls(room))
    {
      fprintf(stderr, "Issue parsing room file: %s.\n", filename);
      return false;
    }
    wall_pairs.clear();
  }

  if (!populateRoomExceptWalls(room))
  {
    fprintf(stderr, "Issue parsing room file: %s.\n", filename);
    return false;
  }

  return true;
}

//======================= DungeonParser ======================//

struct SubRenderable : public Renderable
{
  void draw(const mat3& projection, const mat3& parent_transform) {}
};

// this function design is pulled from http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
void read_directory(const char* dirname, std::vector<std::string>& vec)
{
  /* this part of the code had to be reverted, as it had a dependency on
  std::filesystem that wasn't building on mac. keeping it for hopefully
  the future.

  struct path_leaf_string
  {
    std::string operator()(const std::experimental::filesystem::directory_entry& entry) const
    {
      return entry.path().relative_path().string();
    }
  };

  std::experimental::filesystem::path p(dirname);
  std::experimental::filesystem::directory_iterator start(p);
  std::experimental::filesystem::directory_iterator end;
  std::transform(start, end, std::back_inserter(vec), path_leaf_string());
  */
}

DungeonParser::DungeonParser() :
  m_room_files()
{
  //read_directory(room_path(""), m_room_files);

  m_room_files.emplace_back(room_path("1.rm"));
  m_room_files.emplace_back(room_path("2.rm"));
  m_room_files.emplace_back(room_path("3.rm"));
  m_room_files.emplace_back(room_path("4.rm"));
}

bool DungeonParser::parseDungeon(std::vector<std::unique_ptr<Room>>& rooms, const char* filename, Dungeon& dungeon)
{
  std::string line;
  std::ifstream file(filename);
  std::vector<std::string> lines;

  while (std::getline(file, line))
  {
    lines.emplace_back(line);
  }

  return parseLines(lines, rooms, dungeon);
}

void transform_positions(vector<Door::door_pair>& positions, SubRenderable& rend)
{
  for (Door::door_pair& pos : positions)
  {
    pos.first = get_world_coords_from_room_coords(pos.first, rend.transform, identity_matrix);
  }
}

bool add_doors_to_dungeon(vector<Door::door_pair>& door_pos, Dungeon& dungeon, vec2 offset, int roomID, Room* room, Room* hallway)
{
  std::vector<std::unique_ptr<Door>> doors;

  // hack to add the door to dungeon
  SubRenderable rend;
  rend.transform_begin();
  rend.transform_translate(offset*2.f);
  rend.transform_scale({ 2.f, 2.f });
  rend.transform_end();

  transform_positions(door_pos, rend);

  for (Door::door_pair& pair : door_pos)
  {
    doors.emplace_back(new Door);
    if (!doors.back()->init({ pair.first.x, pair.first.y + 35.f }, pair.second))
    {
      fprintf(stderr, "Error adding doors to dungeon.\n");
      return false;
    }
    doors.back()->set_scale({ 2.f, 2.f });
  }

  Door* the_door = doors[0].get(); // for now there's only one door for each room, this will hopefully change in the future
  dungeon.add_adjacency(roomID, { hallway, the_door }); // adjacency going from room -> hallway
  dungeon.add_adjacency(-1, { room, the_door }); // adjacency going from hallway -> room (-1 is special id for hallway)

  if (!dungeon.add_doors(doors))
  {
    // TODO ERROR
    return false;
  }

  return true;
}

bool DungeonParser::parseLines(std::vector<std::string>& lines, std::vector<std::unique_ptr<Room>>& rooms, Dungeon& dungeon)
{
  size_t row, column;
  int num_rooms = 0;
  vec2 offset = { 0.f,0.f };
  RoomParser roomParser;

  rooms.clear();
  rooms.emplace_back(new Room);
  Room* hallway = rooms.back().get();
  hallway->setRoomID(-1);
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

      if (ch == HALLWAY)
      {
        bool topRow = (row == 0) || (lines[row-1][column] == EMPTY) || (lines[row - 1][column] == SPACE);
        bool bottomRow = (row == lines.size() - 1) || (lines[row + 1][column] == EMPTY) || (lines[row + 1][column] == SPACE);
        bool startColumn = (column == 0) || (line[column - 1] == EMPTY) || (line[column - 1] == SPACE);
        bool endColumn = (column == line.size() - 1) || (line[column + 1] == EMPTY) || (line[column + 1] == SPACE);
        
        addHallwayHelper(*hallway, offset, topRow, bottomRow, startColumn, endColumn);
      }
      else if (ch == EMPTY || ch == SPACE)
      {
        // do nothing! We let the offset increase
      }
      else if (ch == ROOM)
      {
        rooms.emplace_back(new Room);
        rooms.back()->init(offset*2.f);
        if (!roomParser.parseRoom(*rooms.back(), m_room_files[num_rooms].c_str()))
        {
          return false;
        }
        rooms.back()->setRoomID(num_rooms);

        if (!add_doors_to_dungeon(roomParser.get_door_pos(), dungeon, offset, num_rooms, rooms.back().get(), hallway))
        {
          //error
          return false;
        }

        ++num_rooms;
        // TODO: change room type to be classroom, office, or bathroom
      }
      else
      {
        /* this part of the code had to be reverted, as it had a dependency on
        std::filesystem that wasn't building on mac. keeping it for hopefully
        the future.

        std::string str;
        size_t num;

        // convert the ch to a number, which we will use to index into the array
        // of room text files
        str += ch; // need to do it this way to ensure it's null terminated for
                   // passing into std::atoi
        num = std::atoi(str.c_str());

        if (num > m_room_files.size() || num == 0) // check that the character passed in is valid
        {
          fprintf(stderr,
            "Error parsing room file. Invalid character %c at line %d, "
            "column %d.\n",
            ch, (int)(row + 1), (int)(column + 1));
          return false;
        }

        rooms.emplace_back(new Room);
        rooms.back()->init(offset*2.f);
        if (!roomParser.parseRoom(*rooms.back(), m_room_files[num-1].c_str()))
        {
          return false;
        }
        rooms.back()->setRoomID(num_rooms);

        if (!add_doors_to_dungeon(roomParser.get_door_pos(), dungeon, offset, num_rooms, rooms.back().get(), hallway))
        {
          // error
          return false;
        }

        ++num_rooms;
        // TODO: change room type to be classroom, office, or bathroom
        */

        fprintf(stderr,
          "Error parsing room file. Invalid character %c at line %d, "
          "column %d.\n",
          ch, (int)(row + 1), (int)(column + 1));
        return false;
      }
      
      offset.x += ROOM_X_OFFSET;      
    } // for (; column < line.size(); ++column)
    offset.y += ROOM_Y_OFFSET;
  } // for (; row < lines.size(); ++row)

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
