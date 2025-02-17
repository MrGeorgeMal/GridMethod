#pragma once
#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <iostream>
#include "Size2D.h"
#include "Matrix2D.h"
#include "Shape2D.h"


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
	// smooth - smooth shape value (antialiasing). Takes a value from 1 to 4
	Rasterizer(Size2D<double> cell, int smooth) : _cell(cell), _smooth(4)
	{
		fixSmoothValue();
	}

	// Constructor.
	// dx - minimum step for x
	// dy - minimum step for y
	// smooth - smooth shape value (antialiasing). Takes a value from 1 to 4
	Rasterizer(double dx, double dy, int smooth) : _cell(dx, dy), _smooth(4)
	{
		fixSmoothValue();
	}

#pragma endregion

#pragma region Public Method

public:

	// Rasterize shape 2D
	Matrix2D<Material> Rasterize(Shape2D& shape2d)
	{
		if (shape2d.getType() == "Line2D")
		{
			Line2D* line2d = dynamic_cast<Line2D*>(&shape2d);
			return RasterizeLine(line2d);
		}
		else if (shape2d.getType() == "Polygon2D" || shape2d.getType() == "Rectangle2D")
		{
			Polygon2D* poligon2d = dynamic_cast<Polygon2D*>(&shape2d);
			return RasterizePolygon(poligon2d);
		}
		else
		{
			return Matrix2D<Material>();
		}
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

	// Rasterize line shape
	Matrix2D<Material> RasterizeLine(Line2D* line2d)
	{
		std::cout << "RasterizeLine" << "\n";
		return Matrix2D<Material>();
	}

	// Rasterize polygon shape
	Matrix2D<Material> RasterizePolygon(Polygon2D* polygon2d)
	{
		std::cout << "RasterizePolygon" << "\n";
		return Matrix2D<Material>();
	}

#pragma endregion


#pragma region Private Members

private:

	// Cell size, describe minimum step for x and y. Using for discretization
	Size2D<double> _cell = Size2D<double>(1.0, 1.0);

	// Smooth value (antialiasing). Takes a value from 1 to 4. 1 - disable smooth
	int _smooth = 4;

#pragma endregion

};


#endif // !RASTERIZER_H
