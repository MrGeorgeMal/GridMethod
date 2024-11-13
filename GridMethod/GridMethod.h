#pragma once
#include "StripStructure.h"

class GridMethod
{
private:
	StripStructure* stripStructure = new StripStructure();
	double _minSizeX = 0.0;
	double _minSizeY = 0.0;
	double _divideMinSizeX = 1.0;
	double _divideMinSizeX = 1.0;
	double _dx = 1.0;
	double _dy = 1.0;

public:
	GridMethod(StripStructure* stripStructure)
	{
		
	}

};