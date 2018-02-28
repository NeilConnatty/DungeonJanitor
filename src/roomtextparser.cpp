// roomtextparser.cpp

#include "roomtextparser.hpp"

#include <fstream>
#include <string>


#define SPACE ' '
#define WALL 'w'
#define FLOOR 'f'
#define PUDDLE 'p'
#define ARTIFACT 'a'
#define JANITOR 'j'
#define HERO 'h'
#define BOSS 'b'
#define DOOR    'd'


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
