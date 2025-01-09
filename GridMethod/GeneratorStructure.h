#pragma once
#include "StripStructure.h"
#include <random>


static class GeneratorStructure
{
	static void Generate(int rows, int cols)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distSignalConductorCount(1, 2);
		std::uniform_int_distribution<int> distScreenConductorCount(0, 2);
		std::uniform_int_distribution<int> distDielectricCount(0, 2);

		int signalConducorCount = distSignalConductorCount(gen);
		int screenConductorCount = distScreenConductorCount(gen);
		int dielectricCount = distDielectricCount(gen);

		for (int i = 0; i < signalConducorCount; i++)
		{
			
		}
	}
};