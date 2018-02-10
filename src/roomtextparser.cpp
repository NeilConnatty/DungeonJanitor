// roomtextparser.cpp

#include "roomtextparser.hpp"

#include <fstream>
#include <string>

#define SPACE ' '
#define WALL 'w'
#define FLOOR 'f'
#define PUDDLE 'p'

bool RoomParser::parseLine(std::string &line, float y, bool first_line) {
  float x = 0.f;
  size_t i = 0;
  wall_edge edge = NONE;
  vec2 tile_dim;

  for (; i < line.size(); ++i) {
    const char &ch = line[i];

    if (first_line) {
      edge = TOP;
    }
    if (i == 0 || i == line.size() - 1) {
      edge = (wall_edge)(edge | VERTICAL);
    }

    if (ch == SPACE) {
      continue;
    } else if (ch == WALL) {
      wall_pairs.push_back({{x, y}, edge});
      tile_dim = Wall::get_dimensions(edge);
      x = x + tile_dim.x;
    } else if (ch == FLOOR) {
      floor_pos.push_back({x, y});
      tile_dim = Floor::get_dimensions();
      x = x + tile_dim.x;
    } else if (ch == PUDDLE) {
      puddle_pos.push_back({x, y});
      floor_pos.push_back({x, y});
      tile_dim = Floor::get_dimensions();
      x = x + tile_dim.x;
    } else {
      fprintf(stderr,
              "Error parsing room file. Invalid character %c at line %d, "
              "column %d.\n",
              ch, (int)y + 1, (int)x + 1);
      return false;
    }
  }

  return true;
}

bool RoomParser::parseRoom(Room &room, const char *filename) {
  std::string line;
  std::ifstream file(filename);

  wall_pairs.clear();
  floor_pos.clear();
  puddle_pos.clear();

  float y = 0.f;
  bool first_line = true;
  while (std::getline(file, line)) {
    if (!parseLine(line, y, first_line)) {
      fprintf(stderr, "Error parsing room file: %s.\n", filename);
      return false;
    }
    y = y + 25.f;

    if (!(room.add_floors(floor_pos) && room.add_walls(wall_pairs) &&
          room.add_cleanables(puddle_pos))) {
      fprintf(stderr, "Issue parsing room file: %s.\n", filename);
      return false;
    }
    wall_pairs.clear();
    floor_pos.clear();
    puddle_pos.clear();
  }

  return true;
}
