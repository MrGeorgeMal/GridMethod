#pragma once
#ifndef GRIDSOLVER_H
#define GRIDSOLVER_H

#include <iostream>
#include "Vector.h"
#include "Matrix2D.h"
#include "Point2D.h"
#include "Rect2D.h"
#include "Types.h"



class GridSolver
{
#pragma region Constructors

public:

	// Base constructor
	GridSolver() {}

#pragma endregion


#pragma region Public Methods

public:

	// Compute linear parameters of strip structure
	// matrix - matrix of rasterized strip structure
	const Matrix2D<Types::LinearParameters>& computeLinearParameters(Matrix2D<Types::CellInfo>& matrix) const;

#pragma endregion


#pragma region Private Methods

private:

	// Locate the position of the conductor cells
	void defineConductorCells(
		Matrix2D<Types::CellInfo>& matrix,
		Vector<Point2D<int>>& condCells,
		Point2D<int> checkableCell) const;

	// Locate the position all of the conductors cells
	Vector<Vector<Point2D<int>>> defineAllConductorsCells(Matrix2D<Types::CellInfo>& matrix) const;

	// Locate the initial points of field propagation
	Vector<Point2D<int>> defineInitialCellsForFieldPropagation(
		const Matrix2D<Types::CellInfo>& matrix,
		const Vector<Vector<Point2D<int>>>& allConductorsCells) const;

	// Determine the configuration of the conductors inclusion
	Matrix2D<double> setupInitialPotential(
		Matrix2D<Types::CellInfo>& matrix,
		const Vector<Vector<Point2D<int>>>& conductorsCells,
		const Vector<bool>& conductorsConfig) const;

	// Compute potential cell
	void computeCellPotential(
		const Matrix2D<Types::CellInfo>& matrix,
		Matrix2D<double>& oldPotentialField,
		Matrix2D<double>& bufferPotentialField,
		Matrix2D<double>& potentialField,
		const Point2D<int>& computedPoint) const;

	// Compute all cells around the edges of rect propagation
	void computeRectPropagation(
		const Matrix2D<Types::CellInfo>& matrix,
		Matrix2D<double>& oldPotentialField,
		Matrix2D<double>& bufferPotentialField,
		Matrix2D<double>& potentialField,
		const Point2D<int>& initPoint,
		const Rect2D<int>& rect) const;

	// Compute potential field
	void computeFieldPotential(
		const Matrix2D<Types::CellInfo>& matrix,
		Matrix2D<double>& potentialField,
		const Vector<Point2D<int>>& initCells) const;

	// Compute capacity for potentialField
	double computeCapacity(
		const Matrix2D<Types::CellInfo>& matrix,
		const Matrix2D<double>& potentialField) const;

#pragma endregion‚Û


#pragma region Private Members

private:

	// relaxationCoeff used for the relaxation method to calculate the potential in the cell
	// 1 - disable relaxation
	// 1.5 - optimal value
	double relaxationCoeff = 1.5;

	// Cumpute accuracy
	// must be 0.0 < value < 1.0
	double accuracy = 0.8;

#pragma endregion

};

#endif GRIDSOLVER_H