// roomtextparser.cpp

#include "roomtextparser.hpp"

#include <fstream>
#include <string>
#include <algorithm>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/types.h>
#include <dirent.h>
#endif

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
#define NULLCH '\0'

// Room types
#define HALLWAY_R 'l'
#define OFFICE_R 'o'
#define CLASS_R 'c'
#define BATH_R 't'

//Offsets
#define ROOM_X_OFFSET 185.f
#define ROOM_Y_OFFSET 160.f;
#define WALL_X_OFFSET 10.f
#define WALL_Y_OFFSET 60.f
#define FLOOR_X_OFFSET 35.f
#define FLOOR_Y_OFFSET 25.f

enum location
{
  NORTH,
  EAST,
  SOUTH,
  WEST
};

struct door_info
{
  vec2 pos;
  Door::door_dir dir;
  location loc;
};

struct room_info
{
  Room* ptr;
  vec2  pos;

  room_info(Room* tptr, vec2 tpos) : ptr(tptr), pos(tpos) {}
};

using room_id = int;
using room_pair = std::pair<room_id, location>;

void parseWallHelper(size_t &i, std::string &line, wall_edge &edge,
                     vec2 &tile_dim, std::vector<Room::wall_pair> &wall_pairs,
                     bool first_line, bool last_line, float& x, float& y) 
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
  if (last_line)
  {
    edge = (wall_edge)(edge | BOTTOM);
  }
  wall_pairs.push_back({ { x, y }, edge });
  tile_dim = Wall::get_dimensions(edge);
  x = x + tile_dim.x;
}

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
    else if (ch == EMPTY)
    {
      x += 10.f;
    }
    else if (ch == WALL) 
    {
      parseWallHelper(i, line, edge, tile_dim, wall_pairs, first_line, last_line, x, y);
    }
    else if (ch == HALLWAY_R)
    {
      room_t = HALLWAY_ROOM;
      parseWallHelper(i, line, edge, tile_dim, wall_pairs, first_line, last_line, x, y);
    }
    else if (ch == OFFICE_R)
    {
      room_t = OFFICE_ROOM;
      parseWallHelper(i, line, edge, tile_dim, wall_pairs, first_line, last_line, x, y);
    }
    else if (ch == BATH_R)
    {
      room_t = BATH_ROOM;
      parseWallHelper(i, line, edge, tile_dim, wall_pairs, first_line, last_line, x, y);
    }
    else if (ch == CLASS_R)
    {
      room_t = CLASS_ROOM;
      parseWallHelper(i, line, edge, tile_dim, wall_pairs, first_line, last_line, x, y);
    }
    else if (ch == FLOOR) 
    {
      floor_pos.push_back({ x, y });
      tile_dim = Floor::get_dimensions();
      x = x + tile_dim.x;
    }
    else if (ch == DOOR)
    {
      if (first_line)
      {
        door_infos.push_back({ {x,y}, Door::VERTICAL, NORTH });
      }
      else if (last_line)
      {
        door_infos.push_back({ { x,y }, Door::VERTICAL, SOUTH });
      }
      else if (i == 0)
      {
        door_infos.push_back({ {x-25.f,y}, Door::HORIZONTAL, WEST });
      }
      else
      {
        door_infos.push_back({ { x,y }, Door::HORIZONTAL, EAST });
      }

      tile_dim = Floor::get_dimensions();
      if (i == 0)
      {
        x = x + tile_dim.x - FLOOR_Y_OFFSET;
      }
      else
      {
        x = x + tile_dim.x;
      }
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
  door_infos.clear();
}

bool RoomParser::populateRoom(Room &room) 
{
  return (room.add_floors(floor_pos) && room.add_cleanables(puddle_pos) &&
          room.add_walls(wall_pairs) &&
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
  room_t = HALLWAY_ROOM;

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
  }

    room.set_room_type(room_t);

  if (!populateRoom(room))
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
#ifdef _MSC_VER
  std::string pattern(dirname);
  pattern.append("\\*");
  WIN32_FIND_DATA data;
  HANDLE hFind;
  if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) 
  {
    do 
    {
      std::string str(room_path());
      str.append(data.cFileName);
      vec.push_back(str.c_str());
    } 
    while (FindNextFile(hFind, &data) != 0);
    FindClose(hFind);
  }
#else
  DIR* dirp = opendir(dirname);
  struct dirent * dp;
  while ((dp = readdir(dirp)) != NULL) 
  {
    std::string str(room_path());
    str.append(dp->d_name);
    vec.push_back(str.c_str());
  }
  closedir(dirp);
  std::sort(vec.begin(), vec.end());
#endif // _MSC_VER
}

DungeonParser::DungeonParser() :
  m_room_files()
{
  read_directory(room_path(""), m_room_files);
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

void transform_positions(vector<door_info>& infos, SubRenderable& rend)
{
  for (door_info& door : infos)
  {
    door.pos = get_world_coords_from_room_coords(door.pos, rend.transform, identity_matrix);
  }
}

bool add_doors_to_dungeon(
    std::unordered_map<room_id, room_info> &room_map,
    std::unordered_map<room_id, std::vector<door_info>> &door_map,
    std::unordered_map<room_id, std::vector<room_pair>> &room_adjacency_map,
    Dungeon &dungeon, int num_rooms, Room *hallway) 
{
  std::vector<std::unique_ptr<Door>> doors;

  for (std::pair<const room_id, room_info>& rm_pair : room_map)
  {
    // hack to add the door to dungeon
    SubRenderable rend;
    rend.transform_begin();
    rend.transform_translate(rm_pair.second.pos);
    rend.transform_scale({ 2.f, 2.f });
    rend.transform_end();

    std::vector<room_pair>& adj_rooms = room_adjacency_map.at(rm_pair.first);
    std::vector<door_info>& door_infos = door_map.at(rm_pair.first);

    transform_positions(door_infos, rend);

    for (door_info& door : door_infos)
    {
      Door* door_ptr;

      bool has_adj_room = false;
      bool dont_add_door = false;
      for (room_pair& adj_room : adj_rooms)
      {
        if (adj_room.second == door.loc)
        {
          if (door.loc == SOUTH || door.loc == WEST)
          {
            dont_add_door = true;
                        // in this case we don't want to add this door.
                        // We'll let the room to the bottom/left do it for us.
                        // This is arbitrary, but makes sure that doors don't
                        // get added to the dungeon twice
          }
          else
          {
            door_ptr = new Door;
            doors.emplace_back(door_ptr);
            if (!doors.back()->init({ door.pos.x, door.pos.y + 35.f }, door.dir))
            {
              fprintf(stderr, "Error adding doors to dungeon.\n");
              return false;
            }
            door_ptr->set_scale({ 2.f, 2.f });

            Room* adj_room_ptr = room_map.at(adj_room.first).ptr;
            dungeon.add_adjacency(rm_pair.first, { adj_room_ptr, door_ptr });
            dungeon.add_adjacency(adj_room.first, { rm_pair.second.ptr, door_ptr });
            has_adj_room = true;
          }
          break;
        }
      }

      if (dont_add_door)
      {
        continue; // we can continue on the the next door
      }

      if (!has_adj_room)
      {
        door_ptr = new Door;
        doors.emplace_back(door_ptr);
        if (!doors.back()->init({ door.pos.x, door.pos.y + 35.f }, door.dir))
        {
          fprintf(stderr, "Error adding doors to dungeon.\n");
          return false;
        }
        door_ptr->set_scale({ 2.f,2.f });

        dungeon.add_adjacency(rm_pair.first, { hallway, door_ptr }); // adjacency going from room -> hallway
        dungeon.add_adjacency(-1, { rm_pair.second.ptr, door_ptr }); // adjacency going from hallway -> room (-1 is special id for hallway)
      }
    }
  }

  if (!dungeon.add_doors(doors))
  {
    // TODO ERROR
    return false;
  }

  return true;
}

bool roomAbove(std::vector<std::string>& lines, size_t row, size_t column)
{
  return row != 0 && lines[row - 1][column] != HALLWAY && lines[row - 1][column] != EMPTY && lines[row - 1][column] != SPACE && lines[row - 1][column] != NULLCH;
}

bool roomBelow(std::vector<std::string>& lines, size_t row, size_t column)
{
  return row < (lines.size() - 1) && lines[row + 1][column] != HALLWAY && lines[row + 1][column] != EMPTY && lines[row + 1][column] != SPACE && lines[row + 1][column] != NULLCH;
}

bool roomLeft(std::vector<std::string>& lines, size_t row, size_t column)
{
  return column != 0 && lines[row][column - 1] != HALLWAY && lines[row][column - 1] != EMPTY && lines[row][column - 1] != SPACE && lines[row][column - 1] != NULLCH;
}

bool roomRight(std::vector<std::string>& lines, size_t row, size_t column)
{
  return column < (lines[row].size() - 1) && lines[row][column + 1] != HALLWAY && lines[row][column + 1] != EMPTY && lines[row][column + 1] != SPACE && lines[row][column + 1] != NULLCH;
}

bool DungeonParser::addAdjacency(
    std::unordered_map<room_id, std::vector<room_pair>> &room_adjacency_map,
    char &adj_ch, room_id id, location loc) 
{
  std::string adj_str;
  room_id adj_num;
  adj_str += adj_ch; // make sure it's null terminated for atoi
  adj_num = std::atoi(adj_str.c_str());

  if (!validRoomID(adj_num))
  {
    fprintf(stderr, "Error parsing dungeon file. Invalid character %c. \n ",
            adj_ch);
    return false;
  }

  auto search = room_adjacency_map.find(id);
  search->second.emplace_back(adj_num, loc);
  return true;
}


bool DungeonParser::buildRooms(std::vector<std::string> &lines,
                               std::vector<std::unique_ptr<Room>> &rooms,
                               Dungeon &dungeon, Room &hallway) 
{
  size_t row, column;
  room_id num_rooms = 0;
  vec2 offset = { 0.f,0.f };

  RoomParser roomParser;
  std::unordered_map<room_id, room_info> room_map;
  std::unordered_map<room_id, std::vector<door_info>> door_map;
  std::unordered_map<room_id, std::vector<room_pair>> room_adjacency_map;

  for (row = 0; row < lines.size(); ++row)
  {
    std::string& line = lines[row];
    offset.x = 0.f;
    for (column = 0; column < line.size(); ++column)
    {
      char& ch = line[column];

      if (ch == HALLWAY)
      {
        // do nothing! We let the offset increase
      }
      else if (ch == EMPTY || ch == SPACE)
      {
        // do nothing! We let the offset increase
      }
      else
      {
        std::string str;
        room_id num;

        // convert the ch to a number, which we will use to index into the array
        // of room text files. We will also set this to be the room's ID
        str += ch; // need to do it this way to ensure it's null terminated for
                   // passing into std::atoi
        num = std::atoi(str.c_str());

        if (!validRoomID(num)) // check that the character passed in is valid
        {
          fprintf(stderr,
            "Error parsing dungeon file. Invalid character %c at line %d, "
            "column %d.\n",
            ch, (int)(row + 1), (int)(column + 1));
          return false;
        }

        vec2 room_pos = offset * 2.f;
        rooms.emplace_back(new Room);
        rooms.back()->init(room_pos, HALLWAY_ROOM);
        if (!roomParser.parseRoom(*rooms.back(), m_room_files[num + 1].c_str()))
        {
          return false;
        }
        rooms.back()->setRoomID(num);

        room_map.emplace(num, room_info(rooms.back().get(), room_pos));
        door_map.emplace(num, roomParser.get_door_info());
        room_adjacency_map.emplace(num, std::vector<room_pair>()); // initialize with empty vector

        // check for room adjacencies
        if (roomAbove(lines, row, column)) 
        {
          char& adj_ch = lines[row - 1][column];
          if (!addAdjacency(room_adjacency_map, adj_ch, num, NORTH))
          {
            return false;
          }
        }
        if (roomBelow(lines, row, column))
        {
          char& adj_ch = lines[row + 1][column];
          if (!addAdjacency(room_adjacency_map, adj_ch, num, SOUTH))
          {
            return false;
          }
        }
        if (roomLeft(lines, row, column))
        {
          char& adj_ch = lines[row][column - 1];
          if (!addAdjacency(room_adjacency_map, adj_ch, num, WEST))
          {
            return false;
          }
        }
        if (roomRight(lines, row, column))
        {
          char& adj_ch = lines[row][column + 1];
          if (!addAdjacency(room_adjacency_map, adj_ch, num, EAST))
          {
            return false;
          }
        }

        ++num_rooms;
      }

      offset.x += ROOM_X_OFFSET;
    } // for (; column < line.size(); ++column)
    offset.y += ROOM_Y_OFFSET;
  } // for (; row < lines.size(); ++row)

  if (!add_doors_to_dungeon(room_map, door_map, room_adjacency_map, dungeon,
                            num_rooms, &hallway)) 
  {
    // error
    return false;
  }

  return true;
}

bool DungeonParser::buildHallway(std::vector<std::string> &lines,
                                 std::vector<std::unique_ptr<Room>> &rooms,
                                 Dungeon &dungeon, Room &hallway) 
{
  size_t row, column;
  vec2 offset = { 0.f,0.f };

  for (row = 0; row < lines.size(); ++row)
  {
    std::string& line = lines[row];
    offset.x = 0.f;
    for (column = 0; column < line.size(); ++column)
    {
      char& ch = line[column];

      if (ch == HALLWAY)
      {
        bool topRow = (row == 0) || (lines[row - 1][column] == EMPTY) || (lines[row - 1][column] == SPACE);
        bool bottomRow = (row == lines.size() - 1) || (lines[row + 1][column] == EMPTY) || (lines[row + 1][column] == SPACE);
        bool startColumn = (column == 0) || (line[column - 1] == EMPTY) || (line[column - 1] == SPACE);
        bool endColumn = (column == line.size() - 1) || (line[column + 1] == EMPTY) || (line[column + 1] == SPACE);

        if (!addHallwayHelper(hallway, offset, topRow, bottomRow, startColumn, endColumn))
        {
          fprintf(stderr, "error building elements of hallway. \n");
          return false;
        }
      }
      else if (ch == EMPTY || ch == SPACE)
      {
        // do nothing! We let the offset increase
      }
      else
      {
        // do nothing! We let the offset increase
      }

      offset.x += ROOM_X_OFFSET;
    } // for (; column < line.size(); ++column)
    offset.y += ROOM_Y_OFFSET;
  } // for (; row < lines.size(); ++row)

  return true;
}

bool DungeonParser::parseLines(std::vector<std::string>& lines, std::vector<std::unique_ptr<Room>>& rooms, Dungeon& dungeon)
{
  rooms.clear();
  rooms.emplace_back(new Room);
  Room* hallway = rooms.back().get();
  hallway->setRoomID(-1);
  if (!hallway->init())
  {
    fprintf(stderr, "Failed to init hallway room. \n");
    return false;
  }

  if (!buildHallway(lines, rooms, dungeon, *hallway))
  {
    fprintf(stderr, "Failed to build hallway in parser. \n");
    return false;
  }
  
  if (!buildRooms(lines, rooms, dungeon, *hallway))
  {
    fprintf(stderr, "Failed to build rooms in parser. \n");
    return false;
  }

  return true;
}

bool DungeonParser::validRoomID(int id)
{
  return (id > 0 && id <= m_room_files.size());
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
