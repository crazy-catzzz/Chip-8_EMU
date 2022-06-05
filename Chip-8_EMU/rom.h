#pragma once

#include <iostream>
#include <fstream>

struct rom {
	std::string name;
	std::ifstream filePath;
};