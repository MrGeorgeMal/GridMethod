#pragma once
#ifndef STRIPSTRUCTURE_H
#define STRIPSTRUCTURE_H

#include <iostream>
#include "Vector.h"
#include "Matrix2D.h"
#include "Shape2D.h"
#include "Rect2D.h"
#include "Point2D.h"
#include "Rasterizer.h"
#include "Tool.h"
#include "GridSolver.h"


class StripStructure
{

#pragma region Constructors

public:

	// Base constructor
	StripStructure() : _rasterizer(new Rasterizer()), _gridSolver(new GridSolver()) { }

#pragma endregion


#pragma region Public Methods

public:

	// Compute electrostatic analysis
	void computeElectroStaticAnalysis();

	// Compute electrodynamic analysis
	void computeElectroDynamicAnalysis();

	// Add shape
	void addShape(Shape2D* shape);

#pragma endregion


#pragma region Private Methods

private:

	// Find or create screen
	Rectangle2D* defineScreenRectangle();

	// Try to find screen
	Rectangle2D* findScreenRectangle() const;

	// Create new screen if could not find it
	Rectangle2D* createScreenRectangle();

	// Get strip structure rect bound
	Rect2D<double> getRectBound() const;

	// Define min structure size
	Size2D<double> defineMinSize() const;

	// Define optimal cell size
	Size2D<double> defineOptimalCellSize() const;

	// Get offset shapes to center [0 ; 0]
	Vector<Shape2D*> getOffsetShapesToCenter() const;

#pragma endregion


#pragma region Private Members

private:

	// Rasterizer
	Rasterizer* _rasterizer;

	// GridSolver
	GridSolver* _gridSolver;

	// Vector of shapes
	Vector<Shape2D*> _shapes;

	// Screen
	Rectangle2D* _screen = nullptr;

	// Determines if the grid is regular (cell width = height) and all cells are the same size
	bool _isRegularGrid = true;

	// Optimal grid size
	Size2D<int> _optimalGridSize = Size2D<int>(100, 100);

	// Gap between shapes and screen
	double _screenDistance = 10.0;

#pragma endregion

};

#endif // !STRIPSTRUCTURE_H
