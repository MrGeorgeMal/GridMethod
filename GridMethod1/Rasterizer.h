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
	// antialiasing - disable / enable antialiasing
	Rasterizer(Size2D<double> cell, bool antialiasing) : _cell(cell), _antialiasing(antialiasing) {}

	// Constructor.
	// dx - minimum step for x
	// dy - minimum step for y
	// antialiasing - disable / enable antialiasing
	Rasterizer(double dx, double dy, bool antialiasing) : _cell(dx, dy), _antialiasing(antialiasing) {}

#pragma endregion

#pragma region Public Method

public:

	// Rasterize shape 2D
	Matrix2D<Material> rasterize(Shape2D& shape2d);

	// Rasterize strip structure
	Matrix2D<Material> rasterize(Vector<Shape2D> shapes);

#pragma endregion


#pragma region Private Methods

private:

	// Get discretize point coordinates
	Point2D<int> discretizePoint(Point2D<double> point);

	// Define matrix size
	Size2D<int> defineMatrixSize(Line2D line2d);

	// Define matrix size
	Size2D<int> defineMatrixSize(Line2D polygon2d);

	// Define matrix size
	Size2D<int> defineMatrixSize(Vector<Shape2D> shapes);

	// Rasterize line shape
	Matrix2D<Material> rasterizeLine(Line2D* line2d);

	// Rasterize polygon shape
	Matrix2D<Material> rasterizePolygon(Polygon2D* polygon2d);

	// Rasterize line, Bresenham method
	Matrix2D<Material> drawLineBresenham(Line2D* line2d);

	// Rasterize line, Wu method
	Matrix2D<Material> drawLineWu(Line2D* line2d);

	// Rasterize polygon, no antialiasing method
	Matrix2D<Material> drawPolygon(Polygon2D* polygon2d);

	// Rasterize polygon, super sampled antialiasing method
	Matrix2D<Material> drawPolygonSSAA(Polygon2D* polygon2d);

#pragma endregion


#pragma region Private Members

private:

	// Cell size, describe minimum step for x and y. Using for discretization
	Size2D<double> _cell = Size2D<double>(1.0, 1.0);

	// Antialiasing
	bool _antialiasing = false;

	// Samples per cell. Takes a value from 1 to 4
	int _samplesPerCell = 4;

#pragma endregion

};


#endif // !RASTERIZER_H
