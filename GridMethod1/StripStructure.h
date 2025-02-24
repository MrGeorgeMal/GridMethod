#pragma once
#ifndef STRIPSTRUCTURE_H
#define STRIPSTRUCTURE_H

#include "Vector.h"
#include "Matrix2D.h"
#include "Shape2D.h"
#include "Rect2D.h"



class StripStructure
{

#pragma region Constructors

public:

	// Base constructor
	StripStructure() {}

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
	Rectangle2D* defineScreenRectangle() const;

	// Try to find screen
	Rectangle2D* findScreenRectangle() const;

	// Create new screen if could not find it
	Rectangle2D* createScreenRectangle() const;

	// Get strip structure rect bound
	Rect2D<double> getRectBound() const;

	// Define min structure size
	Size2D<double> defineMinSize() const;

	// Define optimal cell size
	Size2D<double> defineOptimalCellSize() const;

	// Get offset shapes to center [0 ; 0]
	const Vector<Shape2D>& getOffsetShapesToCenter() const;

#pragma endregion


#pragma region Private Members

private:

	// Vector of shapes
	Vector<Shape2D> _shapes;

	// Gap between shapes and screen
	double _screenDistance = 10.0;

	// Potential mapping (matrix of potential)
	Matrix2D<float> _potentialMap;

	// Dielectric mapping
	Matrix2D<float> _dielectricMap;

	// Conductor mapping
	Matrix2D<bool> _conductorMap;

#pragma endregion


};

#endif // !STRIPSTRUCTURE_H
