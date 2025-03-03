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
	Matrix2D<bool> defineConductorsConfiguration(const int conductorsCount) const;

	// Compute potential cell
	// u - current potential
	// uold - potential at the previous iteration
	// ul - left potential
	// ut - top potential
	// ur - right potential
	// ub - bottom potential
	// el - left dielectrical value
	// et - top dielectrical value
	// er - right dielectrical value
	// eb - bottom dielectrical value
	double computeCellPotential(
		const double u,
		const double uold,
		const double ul,
		const double ut,
		const double ur,
		const double ub,
		const double el,
		const double et,
		const double er,
		const double eb) const;

	// Compute potential field
	const Matrix2D<double>& computeFieldPotential(const Matrix2D<Types::CellInfo>& matrix) const;

	// Compute energy
	double computeEnergy(const Matrix2D<Types::CellInfo>& matrix) const;

#pragma endregion


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