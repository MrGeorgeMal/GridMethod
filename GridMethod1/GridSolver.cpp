#include "GridSolver.h"



// Compute linear parameters of strip structure
// matrix - matrix of rasterized strip structure
const Matrix2D<Types::LinearParameters>& GridSolver::computeLinearParameters(const Matrix2D<Types::CellInfo>& matrix) const
{
	Vector<Vector<Point2D<int>>> condCells = defineAllConductorsCells(matrix);
}



// Locate the position of the conductor cells
const Vector<Point2D<int>>& GridSolver::defineConductorCells(
	const Matrix2D<Types::CellInfo>& matrix, 
	Vector<Point2D<int>> condCells, 
	Point2D<int> checkableCell) const
{

}



// Locate the position all of the conductors cells
const Vector<Vector<Point2D<int>>>& GridSolver::defineAllConductorsCells(const Matrix2D<Types::CellInfo>& matrix) const
{
	Vector<Vector<Point2D<int>>> allCondsCells;

	// check all matrix cells
	for (int y = 0; y < matrix.getRows(); y++)
	{
		for (int x = 0; x < matrix.getCols(); x++)
		{

			// if cell is signal conductor, run recursion method for define all cells for this conductor
			if (matrix[y][x].isConductor && matrix[y][x].potential == 1.0)
			{
				bool isNewConductor = true;
				for (int i = 0; i < allCondsCells.getLength(); i++)
				{
					for (int j = 0; j < allCondsCells[i].getLength(); j++)
					{

						// if the current matrix cell already belongs to another conductor, skip it
						if (allCondsCells[i][j].x == x && allCondsCells[i][j].y == y)
						{
							isNewConductor = false;
							break;
						}

					}

					if (!isNewConductor)
					{
						break;
					}
					else
					{
						Vector<Point2D<int>> condCells;
						condCells = defineConductorCells(matrix, condCells);
						allCondsCells.add(condCells);
					}

				}
			}

		}
	}

	return allCondsCells;
}



// Locate the initial points of field propagation
const Vector<Point2D<int>>& GridSolver::defineInitialCellsForFieldPropagation(const Matrix2D<Types::CellInfo>& matrix) const
{

}



// Compute potential cell
double GridSolver::computeCellPotential(
	const double u,
	const double uold,
	const double ul,
	const double ut,
	const double ur,
	const double ub,
	const double el,
	const double et,
	const double er,
	const double eb) const
{

}



// Compute potential field
const Matrix2D<double>& GridSolver::computeFieldPotential(const Matrix2D<Types::CellInfo>& matrix) const
{

}



// Compute energy
double GridSolver::computeEnergy(const Matrix2D<Types::CellInfo>& matrix) const
{

}
