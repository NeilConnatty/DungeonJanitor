// roomtextparser.cpp

#include "roomtextparser.hpp"

#include <fstream>
#include <string>

#define SPACE ' '
#define WALL 'w'
#define FLOOR 'f'
#define PUDDLE 'p'

bool RoomParser::parseLine(std::string& line, float y)
{
	float x = 0.f;

	for (char& ch : line)
	{
		if (ch == SPACE)
		{
			continue;
		}
		else if (ch == WALL)
		{
			wall_pos.push_back({ x, y });
		}
		else if (ch == FLOOR)
		{
			floor_pos.push_back({ x, y });
		}
		else if (ch == PUDDLE)
		{
			puddle_pos.push_back({ x, y });
			floor_pos.push_back({ x, y });
		}
		else
		{
			fprintf(stderr, "Error parsing room file. Invalid character %c at line %d, column %d.\n", ch, (int)y + 1, (int)x + 1);
			return false;
		}

		++x;
	}

	return true;
}

bool RoomParser::parseRoom(Room& room, const char* filename)
{
	std::string line;
	std::ifstream file(filename);

	wall_pos.clear();
	floor_pos.clear();

	float y = 0.f;
	while (std::getline(file, line))
	{
		if (!parseLine(line, y))
		{
			fprintf(stderr, "Error parsing room file: %s.\n", filename);
			return false;
		}
		++y;
	}

	if (!(room.add_floors(floor_pos) && room.add_walls(wall_pos) && room.add_cleanables(puddle_pos)))
	{
		fprintf(stderr, "Issue parsing room file: %s.\n", filename);
		return false;
	}

	return true;
}