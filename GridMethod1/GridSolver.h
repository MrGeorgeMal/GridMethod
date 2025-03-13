#pragma once
#ifndef GRIDSOLVER_H
#define GRIDSOLVER_H

#include <iostream>
#include <chrono>

#include "Vector.h"
#include "Matrix2D.h"
#include "Point2D.h"
#include "Rect2D.h"
#include "Types.h"
#include "MatrixTool.h"


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
	Matrix2D<Types::LinearParameters> computeLinearParameters(const Matrix2D<Types::CellInfo>& matrix) const;

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
		Matrix2D<double>& bufferPotentialField,
		Matrix2D<double>& potentialField,
		const Point2D<int>& computedPoint) const;

	// Compute all cells around the edges of rect propagation
	void computeRectPropagation(
		const Matrix2D<Types::CellInfo>& matrix,
		Matrix2D<double>& bufferPotentialField,
		Matrix2D<double>& potentialField,
		const Point2D<int>& initPoint,
		const Rect2D<int>& rect) const;

	// Compute potential field
	// return iterations count
	int computeFieldPotential(
		const Matrix2D<Types::CellInfo>& matrix,
		Matrix2D<double>& potentialField,
		const Vector<Point2D<int>>& initCells) const;

	// Compute capacity and conductance for potential field
	// return point from to value: [capacity ; conductance]
	Point2D<double> computeCapacityAndConductance(
		const Matrix2D<Types::CellInfo>& matrix,
		const Matrix2D<double>& potentialField) const;

	// Compute linear capacity matrix
	void computeLinearCapacityAndConductanceMatrix(
		Matrix2D<Types::CellInfo>& matrix,
		const Vector<Vector<Point2D<int>>>& condCells,
		const Vector<Point2D<int>>& initCells,
		const Vector<Point2D<int>>& symmetryConductors,
		Matrix2D<Types::LinearParameters>& linearParam,
		bool isAirFill) const;

	// Check structure on symmetry
	// return symmetry point: left and right X coordinate - Point(leftX ; rightX)
	// left and right coordinates may be equal
	// return Point(0 ; 0) if structure has no symmetry
	Point2D<int> defineVerticalSymmetryPoint(const Matrix2D<Types::CellInfo>& matrix) const;

	// Check structure on symmetry
	// return symmetry point: bottom and top Y coordinate - Point(bottomX ; topY)
	// bottom and top coordinates may be equal
	// return Point(0 ; 0) if structure has no symmetry
	Point2D<int> defineHorizontalSymmetryPoint(const Matrix2D<Types::CellInfo>& matrix) const;

	// Define pair of symmetrycal conductors
	// return point: first and numbers - numbers of symmetrical conductors
	// return Point2D(-1 ; -1) if there is no symmetry
	Vector<Point2D<int>> defineVerticalSymmetyConductors(
		const Vector<Vector<Point2D<int>>>& conductorsCells,
		const Point2D<int>& symmetryPoint) const;

	// Define pair of symmetrycal conductors
	// return point: first and numbers - numbers of symmetrical conductors
	// return Point2D(-1 ; -1) if there is no symmetry
	Vector<Point2D<int>> defineHorizontalSymmetyConductors(
		const Vector<Vector<Point2D<int>>>& conductorsCells,
		const Point2D<int>& symmetryPoint) const;

	// Print result info
	void printResultInfo(
		const Point2D<int>& verticalSymmetryPoint,
		const Point2D<int>& horizontalSymmetryPoint,
		const Vector<Point2D<int>>& verticalSymmetryConductors,
		const Vector<Point2D<int>>& horizontalSymmetryConductors,
		const Vector<Point2D<int>>& symmetryConductors,
		const Matrix2D<Types::LinearParameters>& linearParam,
		Matrix2D<Types::PrimaryParameters> primaryParameters) const;

	// Draw potential field
	void drawField(const Matrix2D<double>& potentialField) const;

#pragma endregion


#pragma region Private Members

private:

	// relaxationCoeff used for the relaxation method to calculate the potential in the cell
	// must be 1.0 < value < 2.0
	double relaxationCoeff = 1.7;

	// Cumpute accuracy
	// must be 0.0 < value < 1.0
	double accuracy = 0.99;
	//double accuracy = 0.9;

#pragma endregion

};

#endif GRIDSOLVER_H