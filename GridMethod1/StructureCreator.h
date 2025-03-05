#pragma once
#ifndef STRUCTURECREATOR_H
#define STRUCTURECREATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include "libs/json.hpp"

#include "StripStructure.h"

using json = nlohmann::json;

namespace StructureCreator
{
	// Create strip structure from json file
	StripStructure createStripStructureFromJson(const char* path)
	{
		StripStructure stripStructure;

		std::ifstream file(path);
		if (!file.is_open())
		{
			throw "StructureCreator::createStripStructureFromJson::Failed to open file";
			return stripStructure;
		}
		
		try
		{
			json config;
			file >> config; // Парсим JSON напрямую из файла
			std::cout << "JSON parse succeeds\n";
		}
		catch (const json::parse_error& e)
		{
			std::cout << "JSON parse error\n" << e.what() << std::endl;
			throw "StructureCreator::createStripStructureFromJson::Parse failed";
		}
		
		return stripStructure;
	}
}

#endif STRUCTURECREATOR_H