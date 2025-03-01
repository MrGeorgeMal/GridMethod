#pragma once
#ifndef GRIDSOLVER_H
#define GRIDSOLVER_H

#include "Vector.h"
#include "Matrix2D.h"
#include "Point2D.h"
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
	const Matrix2D<Types::LinearParameters>& computeLinearParameters(Matrix2D<Types::CellInfo>& matrix);

#pragma endregion


#pragma region Private Methods

private:

	// Locate the position of the conductor cells
	const Vector<Point2D<int>>& defineConductorPositionCells(Matrix2D<Types::CellInfo>& matrix);

	// Locate the initial points of field propagation
	const Vector<Point2D<int>>& defineInitialCellsForFieldPropagation(Matrix2D<Types::CellInfo>& matrix);

	// Determine the configuration of the conductor inclusion
	const Matrix2D<bool>& defineConductorsConfiguration();

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
		double u,
		double uold,
		double ul,
		double ut,
		double ur,
		double ub,
		double el,
		double et,
		double er,
		double eb);

	// Compute potential field
	const Matrix2D<double>& computeFieldPotential(Matrix2D<Types::CellInfo>& matrix);

	// Compute energy
	double computeEnergy(Matrix2D<Types::CellInfo>& matrix);

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