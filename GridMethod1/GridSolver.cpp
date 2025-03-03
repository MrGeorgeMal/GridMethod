#include "GridSolver.h"



// Compute linear parameters of strip structure
// matrix - matrix of rasterized strip structure
const Matrix2D<Types::LinearParameters>& GridSolver::computeLinearParameters(Matrix2D<Types::CellInfo>& matrix) const
{
	Vector<Vector<Point2D<int>>> condCells = defineAllConductorsCells(matrix);
	Vector<Point2D<int>> initCells = defineInitialCellsForFieldPropagation(matrix, condCells);
	Matrix2D<bool>configConductors = defineConductorsConfiguration(condCells.getLength());

	std::cout << "Conductors count: " << condCells.getLength() << "\n";
	std::cout << "Conductors psotions: " << "\n";
	for (int i = 0; i < condCells.getLength(); i++)
	{
		std::cout << i + 1 <<") ";
		for (int j = 0; j < condCells[i].getLength(); j++)
		{
			std::cout << condCells[i][j];
		}
		std::cout << "\n";
	}

	std::cout << "\nInitial cells for field propagation: " << "\n";
	for (int i = 0; i < initCells.getLength(); i++)
	{
		std::cout << "For " << i + 1 << " conductor: " << initCells[i] << "\n";
	}
	std::cout << "\nConductors configuration matrix: " << "\n";
	std::cout << configConductors << "\n";

	return Matrix2D<Types::LinearParameters>();
}



// Locate the position of the conductor cells
void GridSolver::defineConductorCells(
	Matrix2D<Types::CellInfo>& matrix, 
	Vector<Point2D<int>>& condCells, 
	Point2D<int> checkableCell) const
{
	// check if this cell has been added before
	for (int i = 0; i < condCells.getLength(); i++)
	{
		if (checkableCell == condCells[i])
		{
			return;
		}
	}

	// if a signal conductor touches a non-signal conductor, the non-signal conductor is switched on
	if (matrix[checkableCell.y][checkableCell.x].isSignalConductor == false)
	{
		matrix[checkableCell.y][checkableCell.x].isSignalConductor = true;
	}

	condCells.add(checkableCell);

	Point2D<int> l = checkableCell + Point2D<int>(-1, 0);
	Point2D<int> t = checkableCell + Point2D<int>(0, 1);
	Point2D<int> r = checkableCell + Point2D<int>(1, 0);
	Point2D<int> b = checkableCell + Point2D<int>(0, -1);
	Point2D<int> lt = checkableCell + Point2D<int>(-1, 1);
	Point2D<int> rt = checkableCell + Point2D<int>(1, 1);
	Point2D<int> rb = checkableCell + Point2D<int>(1, -1);
	Point2D<int> lb = checkableCell + Point2D<int>(-1, -1);

	if (matrix[l.y][l.x].isConductor)
	{
		defineConductorCells(matrix, condCells, l);
	}
	if (matrix[t.y][t.x].isConductor)
	{
		defineConductorCells(matrix, condCells, t);
	}
	if (matrix[r.y][r.x].isConductor)
	{
		defineConductorCells(matrix, condCells, r);
	}
	if (matrix[b.y][b.x].isConductor)
	{
		defineConductorCells(matrix, condCells, b);
	}
	if (matrix[lt.y][lt.x].isConductor)
	{
		defineConductorCells(matrix, condCells, lt);
	}
	if (matrix[rt.y][rt.x].isConductor)
	{
		defineConductorCells(matrix, condCells, rt);
	}
	if (matrix[rb.y][rb.x].isConductor)
	{
		defineConductorCells(matrix, condCells, rb);
	}
	if (matrix[lb.y][lb.x].isConductor)
	{
		defineConductorCells(matrix, condCells, lb);
	}
}



// Locate the position all of the conductors cells
Vector<Vector<Point2D<int>>> GridSolver::defineAllConductorsCells(Matrix2D<Types::CellInfo>& matrix) const
{
	Vector<Vector<Point2D<int>>> allCondsCells;

	// check all matrix cells
	for (int y = 1; y < matrix.getRows() - 1; y++)
	{
		for (int x = 1; x < matrix.getCols() - 1; x++)
		{

			// if cell is signal conductor, run recursion method for define all cells for this conductor
			if (matrix[y][x].isSignalConductor)
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
				}

				if (isNewConductor)
				{
					Vector<Point2D<int>> condCells;
					defineConductorCells(matrix, condCells, Point2D<int>(x, y));
					allCondsCells.add(condCells);
				}
			}

		}
	}

	return allCondsCells;
}



// Locate the initial points of field propagation
Vector<Point2D<int>> GridSolver::defineInitialCellsForFieldPropagation(
	const Matrix2D<Types::CellInfo>& matrix,
	const Vector<Vector<Point2D<int>>>& allConductorsCells) const
{
	// define all conductors rects
	Vector<Rect2D<int>> conductorsRects;

	for (int i = 0; i < allConductorsCells.getLength(); i++)
	{
		int l = allConductorsCells[i][0].x;
		int t = 0;
		int r = 0;
		int b = allConductorsCells[i][0].y;

		for (int j = 0; j < allConductorsCells[i].getLength(); j++)
		{
			if (allConductorsCells[i][j].x < l)
			{
				l = allConductorsCells[i][j].x;
			}
			if (allConductorsCells[i][j].x > r)
			{
				r = allConductorsCells[i][j].x;
			}
			if (allConductorsCells[i][j].y > t)
			{
				t = allConductorsCells[i][j].y;
			}
			if (allConductorsCells[i][j].y < b)
			{
				b = allConductorsCells[i][j].y;
			}
		}
		Rect2D<int> conductorRect(l, t, r, b);
		conductorsRects.add(conductorRect);
	}

	// define initial cells for field propagation 
	Vector<Point2D<int>> cellsForFieldPropagation(conductorsRects.getLength());

	for (int i = 0; i < conductorsRects.getLength(); i++)
	{
		// find the conductors count in each row and column
		// only the conductors related to the current conductor conductorsRects[i] are counted
		Vector<int> conductorCountInRow(conductorsRects[i].top - conductorsRects[i].bottom  + 1);
		Vector<int> conductorCountInColumn(conductorsRects[i].right - conductorsRects[i].left + 1);
		int k = 0;
		for (int y = conductorsRects[i].bottom; y <= conductorsRects[i].top; y++)
		{
			conductorCountInRow[k] = 0;
			for (int x = conductorsRects[i].left; x <= conductorsRects[i].right; x++)
			{
				for (int j = 0; j < allConductorsCells[i].getLength(); j++)
				{
					if (Point2D<int>(x, y) == allConductorsCells[i][j])
					{
						conductorCountInRow[k]++;
						break;
					}
				}
			}
			k++;
		}
		k = 0;
		for (int x = conductorsRects[i].left; x <= conductorsRects[i].right; x++)
		{
			conductorCountInColumn[k] = 0;
			for (int y = conductorsRects[i].bottom; y <= conductorsRects[i].top; y++)
			{
				for (int j = 0; j < allConductorsCells[i].getLength(); j++)
				{
					if (Point2D<int>(x, y) == allConductorsCells[i][j])
					{
						conductorCountInColumn[k]++;
						break;
					}
				}
			}
			k++;
		}

		// find maximum conductors count for rows and columns
		int maxConductorsInRows = 0;
		int maxConductorsInColumns = 0;

		for (int j = 0; j < conductorCountInRow.getLength(); j++)
		{
			if (maxConductorsInRows < conductorCountInRow[j])
			{
				maxConductorsInRows = conductorCountInRow[j];
			}
		}
		for (int j = 0; j < conductorCountInColumn.getLength(); j++)
		{
			if (maxConductorsInColumns < conductorCountInColumn[j])
			{
				maxConductorsInColumns = conductorCountInColumn[j];
			}
		}

		// find rows and columns with maximum conductors count
		Vector<int> bestRows;
		Vector<int> bestColumns;

		for (int j = 0; j < conductorCountInRow.getLength(); j++)
		{
			if (conductorCountInRow[j] == maxConductorsInRows)
			{
				bestRows.add(conductorsRects[i].bottom + j);
			}
		}
		for (int j = 0; j < conductorCountInColumn.getLength(); j++)
		{
			if (conductorCountInColumn[j] == maxConductorsInColumns)
			{
				bestColumns.add(conductorsRects[i].left + j);
			}
		}

		// find best cell coordinates for field propagation
		int x = round((bestColumns[bestColumns.getLength() - 1] + bestColumns[0]) / 2);
		int y = round((bestRows[bestRows.getLength() - 1] + bestRows[0]) / 2);

		if (matrix[y][x].isSignalConductor == false)
		{
			x = bestColumns[0];
			y = bestRows[0];
		}

		cellsForFieldPropagation[i].x = x;
		cellsForFieldPropagation[i].y = y;
	}

	return cellsForFieldPropagation;
}



// Determine the configuration of the conductors inclusion
Matrix2D<bool> GridSolver::defineConductorsConfiguration(const int conductorsCount) const
{	
	Vector<Vector<bool>> config;

	// 2^conductorsCount (bit shift)
	int rows = 1 << conductorsCount;

	for (int i = 0; i < rows; i++)
	{
		Vector<bool> configLine;
		for (int j = conductorsCount - 1; j >= 0; j--)
		{
			bool current = (i >> j) & 1;
			configLine.add(current);
		}

		bool skipThisConfigLine = true;

		// skip line if all elements are zero
		for (int j = 0; j < configLine.getLength(); j++)
		{
			if (configLine[j] == true)
			{
				skipThisConfigLine = false;
				break;
			}
		}

		// skip line if is mirror for other line
		bool isMirror = true;

		if (config.getLength() == 0)
		{
			isMirror = false;
		}

		for (int j = 0; j < config.getLength(); j++)
		{
			// dont check on mirror if there only one conductor is signal
			int signalConductorcount = 0;
			for (int k = 0; k < configLine.getLength(); k++)
			{
				if (configLine[k] == true)
				{
					signalConductorcount++;
				}
			}
			if (signalConductorcount == 1)
			{
				isMirror = false;
				break;
			}

			// check on mirror
			isMirror = true;
			int count = configLine.getLength() - 1;

			for (int k = 0; k < config[j].getLength(); k++)
			{
				bool first = configLine[count];
				bool second = config[j][k];
				if (isMirror == true && first != second)
				{
					isMirror = false;
					break;
				}
				count--;
			}
			if (isMirror == true)
			{
				break;
			}
		}
		if (isMirror)
		{
			skipThisConfigLine = true;
		}

		// skip line if the number of nearby conductors is equal to the same number of nearby conductors in another line
		// if the line has elements with 0 between them - such a line is not checked



		if (skipThisConfigLine == false)
		{
			config.add(configLine);
		}
	}

	Matrix2D<bool> matrixConfig(config.getLength(), config[0].getLength());

	for (int i = 0; i < config.getLength(); i++)
	{
		for (int j = 0; j < config[i].getLength(); j++)
		{
			matrixConfig[i][j] = config[i][j];
		}
	}

	return matrixConfig;
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
	return 0.0;
}



// Compute potential field
const Matrix2D<double>& GridSolver::computeFieldPotential(const Matrix2D<Types::CellInfo>& matrix) const
{
	return Matrix2D<double>();
}



// Compute energy
double GridSolver::computeEnergy(const Matrix2D<Types::CellInfo>& matrix) const
{
	return 0.0;
}
