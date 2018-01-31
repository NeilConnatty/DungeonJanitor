// roomtextparser.cpp

#include "roomtextparser.hpp"

#include <fstream>
#include <string>

void Parser::parseRoom(Room& room, const char* filename)
{
	std::string line;
	std::ifstream file(filename);

	while (std::getline(file, line))
	{

	}
}