#include "ReadFromExtern.h"
#include <fstream>
#include <iostream>

bool LoadCollision(
	const char *file_path,
	std::vector<float> & collisionX,
	std::vector<float> & collisionZ
	)
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}

	while (!fileStream.eof())
	{
		char buf[256];
		fileStream.getline(buf, 256);
		if (strncmp("x ", buf, 2) == 0)
		{
			float positionX;
			sscanf_s((buf + 2), "%f", &positionX);
			collisionX.push_back(positionX);
		}
		else if (strncmp("z ", buf, 2) == 0)
		{
			float positionZ;
			sscanf_s((buf + 2), "%f", &positionZ);
			collisionZ.push_back(positionZ);
		}
	}
	fileStream.close();
	return true;
}

bool LoadText(const char *file_path, std::vector<std::string> & List)
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}
	while (!fileStream.eof())
	{
		char buf[256];
		fileStream.getline(buf, 256);
		List.push_back(buf);
	}
	fileStream.close();
	return true;
}