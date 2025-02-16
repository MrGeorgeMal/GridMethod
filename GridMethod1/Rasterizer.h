#pragma once
#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "Size2D.h"



class Rasterizer
{
#pragma region Constructors

public:

	// Base constuctor
	Rasterizer() {}

	// Constructor.
	// cell - cell size, describe minimum step for x and y. Using for discretization
	Rasterizer(Size2D<double> cell): _cell(cell) {}

	// Constructor.
	// cell - cell size, describe minimum step for x and y. Using for discretization
	// Smooth shape value. Takes a value from 1 to 4
	Rasterizer(Size2D<double> cell, int smooth) : _cell(cell), _smooth(4)
	{
		fixSmoothValue();
	}

	// Constructor.
	// dx - minimum step for x
	// dy - minimum step for y
	// Smooth shape value. Takes a value from 1 to 4
	Rasterizer(double dx, double dy, int smooth) : _cell(dx, dy), _smooth(4)
	{
		fixSmoothValue();
	}

#pragma endregion


#pragma region Private Methods

private:

	// Check and change smooth for correctness value.
	// Return false if value has been changed
	// Return true if value is correctness
	bool fixSmoothValue()
	{
		if (_smooth < 1)
		{
			_smooth = 1;
			return false;
		}
		if (_smooth > 4)
		{
			_smooth = 4;
			return false;
		}
		return true;
	}

#pragma endregion


#pragma region Private Members

private:

	// Cell size, describe minimum step for x and y. Using for discretization
	Size2D<double> _cell = Size2D<double>(1.0, 1.0);

	// Smooth value. Takes a value from 1 to 4
	int _smooth = 4;

#pragma endregion

};


#endif // !RASTERIZER_H
