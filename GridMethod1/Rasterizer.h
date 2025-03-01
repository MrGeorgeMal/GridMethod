#pragma once
#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <iostream>
#include "Size2D.h"
#include "Matrix2D.h"
#include "Shape2D.h"
#include "Material.h"
#include "Point2D.h"
#include "Vector.h"
#include "Tool.h"
#include "Types.h"

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

#pragma endregion


#pragma region Public Method

public:

	// Rasterize strip structure
	const Matrix2D<Types::CellInfo> rasterize(const Vector<Shape2D*>& shapes) const;

#pragma endregion


#pragma region Private Methods

private:

	// Find screen in shapes
	Rectangle2D* findScreen(const Vector<Shape2D*>& shapes) const;

	// Define matrix size
	Size2D<int> defineMatrixSize(const Vector<Shape2D*>& shapes) const;

	// Get updated cell info based on old cell info and new material
	Types::CellInfo getUpdatedCellInfo(Types::CellInfo oldCellInfo, Material* material) const;

	// Rasterize line shape
	void rasterizeLine(const Line2D* line2d, Matrix2D<Types::CellInfo>& matrix) const;

	// Rasterize polygon shape
	void rasterizePolygon(const Polygon2D* polygon2d, Matrix2D<Types::CellInfo>& matrix) const;

	// Plot matrix element (set cell info)
	void plot(Matrix2D<Types::CellInfo>& matrix, int x, int y, Material* material) const;

	// Check point if it inside the polygon
	bool isInsidePolygon(int x, int y, const Vector<Point2D<double>>& polygonPoints) const;

	// Rasterize line, Bresenham method
	void drawLineBresenham(const Line2D* line2d, Matrix2D<Types::CellInfo>& matrix) const;

	// Rasterize line, Wu method
	void drawLineWu(const Line2D* line2d, Matrix2D<Types::CellInfo>& matrix) const;

	// Rasterize polygon, no antialiasing method
	void drawPolygon(const Polygon2D* polygon2d, Matrix2D<Types::CellInfo>& matrix) const;

	// Rasterize polygon, super sampled antialiasing method
	void drawPolygonSSAA(const Polygon2D* polygon2d, Matrix2D<Types::CellInfo>& matrix) const;

#pragma endregion


#pragma region Getter Setter

public:

	Size2D<double> getCell() { return _cell; }

	void setCell(Size2D<double> cell) { _cell = cell; }

	bool getAntialiasing() { return _antialiasing; }

	void setAntialiasing() { _antialiasing = _antialiasing; }

	int getSamplesPerCell() { return _samplesPerCell; }

	void setSamplesPerCell(int samples) { _samplesPerCell = samples; }

#pragma endregion


#pragma region Private Members

private:

	// Cell size, describe minimum step for x and y. Using for discretization
	Size2D<double> _cell = Size2D<double>(1.0, 1.0);

	// Antialiasing
	bool _antialiasing = false;

	// Samples per cell. Takes a value from 1 to 4
	int _samplesPerCell = 4;

	// Cell number for screen
	int _screenBorder = 1;

#pragma endregion

};


#endif // !RASTERIZER_H
