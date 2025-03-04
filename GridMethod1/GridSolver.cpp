#include "GridSolver.h"



// Compute linear parameters of strip structure
// matrix - matrix of rasterized strip structure
const Matrix2D<Types::LinearParameters>& GridSolver::computeLinearParameters(Matrix2D<Types::CellInfo>& matrix) const
{
	// defines
	Vector<Vector<Point2D<int>>> condCells = defineAllConductorsCells(matrix);
	Vector<Point2D<int>> initCells = defineInitialCellsForFieldPropagation(matrix, condCells);

	// compute linear capacity
	Matrix2D<Types::LinearParameters> linearParam(condCells.getLength(), condCells.getLength());

	// compute diagonal elements in linear capacity matrix (full capacity for each signal conductor)
	Vector<bool> conductorsConfig(condCells.getLength());

	for (int i = 0; i < condCells.getLength(); i++)
	{
		conductorsConfig[i] = true;

		Matrix2D<double>potentialField = setupInitialPotential(matrix, condCells, conductorsConfig);
		computeFieldPotential(matrix, potentialField, initCells);
		linearParam[i][i].C = computeCapacity(matrix, potentialField);

		conductorsConfig[i] = false;
	}

	// compute other elements in linear capacity matrix
	for (int i = 0; i < condCells.getLength(); i++)
	{
		for (int j = 0; j < conductorsConfig.getLength(); j++)
		{
			conductorsConfig[j] = false;
		}
		conductorsConfig[i] = true;

		for (int j = i + 1; j < conductorsConfig.getLength(); j++)
		{
			conductorsConfig[j] = true;

			Matrix2D<double>potentialField = setupInitialPotential(matrix, condCells, conductorsConfig);
			computeFieldPotential(matrix, potentialField, initCells);
			linearParam[i][j].C = computeCapacity(matrix, potentialField);

			conductorsConfig[j] = false;
		}
	}

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

	std::cout << "\nLinear capacity matrix: " << "\n";
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		for (int j = 0; j < linearParam.getCols(); j++)
		{
			std::cout << linearParam[i][j].C << "\t";
		}
		std::cout << "\n\n";
	}

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
Matrix2D<double> GridSolver::setupInitialPotential(
	Matrix2D<Types::CellInfo>& matrix,
	const Vector<Vector<Point2D<int>>>& conductorsCells,
	const Vector<bool>& conductorsConfig) const
{
	Matrix2D<double> potentialField(matrix.getRows(), matrix.getCols());

	for (int y = 0; y < potentialField.getRows(); y++)
	{
		for (int x = 0; x < potentialField.getCols(); x++)
		{
			potentialField[y][x] = Types::initLowPotential;
		}
	}

	for (int i = 0; i < conductorsCells.getLength(); i++)
	{
		if (conductorsConfig[i] == true)
		{
			for (int j = 0; j < conductorsCells[i].getLength(); j++)
			{
				int x = conductorsCells[i][j].x;
				int y = conductorsCells[i][j].y;
				potentialField[y][x] = Types::initHightPotential;
			}
		}
	}

	return potentialField;
}




// Compute potential cell
void GridSolver::computeCellPotential(
	const Matrix2D<Types::CellInfo>& matrix,
	Matrix2D<double>& oldPotentialField,
	Matrix2D<double>& bufferPotentialField,
	Matrix2D<double>& potentialField,
	const Point2D<int>& computedPoint) const
{
	int rows = potentialField.getRows() - 1;
	int cols = potentialField.getRows() - 1;
	int x = computedPoint.x;
	int y = computedPoint.y;

	if (x > 0 && x < cols &&
		y > 0 && y < rows)
	{
		if (matrix[y][x].isConductor == false)
		{
			double lu = potentialField[y][x - 1];
			double tu = potentialField[y + 1][x];
			double ru = potentialField[y][x + 1];
			double bu = potentialField[y - 1][x];
			double le = matrix[y][x - 1].dielectricValue;
			double te = matrix[y + 1][x].dielectricValue;
			double re = matrix[y][x + 1].dielectricValue;
			double be = matrix[y - 1][x].dielectricValue;
			double oldu = oldPotentialField[y][x];
			double u = 0.25 * (tu + bu + lu + ru);
			potentialField[y][x] = relaxationCoeff * (u);

		}
	}

	potentialField[x][y] = computeCellPotential(
		oldPotentialField[lcenter.x][lcenter.y],
		potentialField[lcenter.x - 1][lcenter.y],
		potentialField[lcenter.x][lcenter.y + 1],
		potentialField[lcenter.x + 1][lcenter.y],
		potentialField[lcenter.x][lcenter.y - 1],
		matrix[lcenter.x - 1][lcenter.y].dielectricValue,
		matrix[lcenter.x][lcenter.y + 1].dielectricValue,
		matrix[lcenter.x + 1][lcenter.y].dielectricValue,
		matrix[lcenter.x][lcenter.y - 1].dielectricValue);

}



// Compute all cells around the edges of rect propagation
void GridSolver::computeRectPropagation(
	const Matrix2D<Types::CellInfo>& matrix,
	Matrix2D<double>& oldPotentialField,
	Matrix2D<double>& bufferPotentialField,
	Matrix2D<double>& potentialField,
	const Point2D<int>& initPoint,
	const Rect2D<int>& rect) const
{
	Point2D<int> lcenter;
	lcenter.x = rect.left;
	lcenter.y = initPoint.y;

	Point2D<int> tcenter;
	tcenter.x = initPoint.x;
	tcenter.y = rect.top;

	Point2D<int> rcenter;
	rcenter.x = rect.right;
	rcenter.y = initPoint.y;

	Point2D<int> bcenter;
	bcenter.x = initPoint.x;
	bcenter.y = rect.bottom;

	Point2D<int> leftTop;
	leftTop.x = lcenter.x;
	leftTop.y = tcenter.y;

	Point2D<int> rightTop;
	rightTop.x = rcenter.x;
	rightTop.y = tcenter.y;

	Point2D<int> rightBottom;
	rightBottom.x = rcenter.x;
	rightBottom.y = bcenter.y;

	Point2D<int> leftBottom;
	leftBottom.x = lcenter.x;
	leftBottom.y = bcenter.y;

	// compute central points on edges
	potentialField[lcenter.x][lcenter.y] = computeCellPotential(
		oldPotentialField[lcenter.x][lcenter.y],
		potentialField[lcenter.x - 1][lcenter.y],
		potentialField[lcenter.x][lcenter.y + 1],
		potentialField[lcenter.x + 1][lcenter.y],
		potentialField[lcenter.x][lcenter.y - 1],
		matrix[lcenter.x - 1][lcenter.y].dielectricValue,
		matrix[lcenter.x][lcenter.y + 1].dielectricValue,
		matrix[lcenter.x + 1][lcenter.y].dielectricValue,
		matrix[lcenter.x][lcenter.y - 1].dielectricValue);

	potentialField[tcenter.x][tcenter.y] = computeCellPotential(
		oldPotentialField[tcenter.x][tcenter.y],
		potentialField[tcenter.x - 1][tcenter.y],
		potentialField[tcenter.x][tcenter.y + 1],
		potentialField[tcenter.x + 1][tcenter.y],
		potentialField[tcenter.x][tcenter.y - 1],
		matrix[tcenter.x - 1][tcenter.y].dielectricValue,
		matrix[tcenter.x][tcenter.y + 1].dielectricValue,
		matrix[tcenter.x + 1][tcenter.y].dielectricValue,
		matrix[tcenter.x][tcenter.y - 1].dielectricValue);
	
	potentialField[rcenter.x][rcenter.y] = computeCellPotential(
		oldPotentialField[rcenter.x][rcenter.y],
		potentialField[rcenter.x - 1][rcenter.y],
		potentialField[rcenter.x][rcenter.y + 1],
		potentialField[rcenter.x + 1][rcenter.y],
		potentialField[rcenter.x][rcenter.y - 1],
		matrix[rcenter.x - 1][rcenter.y].dielectricValue,
		matrix[rcenter.x][rcenter.y + 1].dielectricValue,
		matrix[rcenter.x + 1][rcenter.y].dielectricValue,
		matrix[rcenter.x][rcenter.y - 1].dielectricValue);

	potentialField[bcenter.x][bcenter.y] = computeCellPotential(
		oldPotentialField[bcenter.x][bcenter.y],
		potentialField[bcenter.x - 1][bcenter.y],
		potentialField[bcenter.x][bcenter.y + 1],
		potentialField[bcenter.x + 1][bcenter.y],
		potentialField[bcenter.x][bcenter.y - 1],
		matrix[bcenter.x - 1][bcenter.y].dielectricValue,
		matrix[bcenter.x][bcenter.y + 1].dielectricValue,
		matrix[bcenter.x + 1][bcenter.y].dielectricValue,
		matrix[bcenter.x][bcenter.y - 1].dielectricValue);

	oldPotentialField[lcenter.x][lcenter.y] = bufferPotentialField[lcenter.x][lcenter.y];
	oldPotentialField[tcenter.x][tcenter.y] = bufferPotentialField[tcenter.x][tcenter.y];
	oldPotentialField[rcenter.x][rcenter.y] = bufferPotentialField[rcenter.x][rcenter.y];
	oldPotentialField[bcenter.x][bcenter.y] = bufferPotentialField[bcenter.x][bcenter.y];

	bufferPotentialField[lcenter.x][lcenter.y] = potentialField[lcenter.x][lcenter.y];
	bufferPotentialField[tcenter.x][tcenter.y] = potentialField[tcenter.x][tcenter.y];
	bufferPotentialField[rcenter.x][rcenter.y] = potentialField[rcenter.x][rcenter.y];
	bufferPotentialField[bcenter.x][bcenter.y] = potentialField[bcenter.x][bcenter.y];
	
	// compute left vertical edge
	for (int y = lcenter.y + 1; y < leftTop.y; y++)
	{
		potentialField[lcenter.x][y] = computeCellPotential(
			oldPotentialField[lcenter.x][y],
			potentialField[lcenter.x - 1][y],
			potentialField[lcenter.x][y + 1],
			potentialField[lcenter.x + 1][y],
			potentialField[lcenter.x][y - 1],
			matrix[lcenter.x - 1][y].dielectricValue,
			matrix[lcenter.x][y + 1].dielectricValue,
			matrix[lcenter.x + 1][y].dielectricValue,
			matrix[lcenter.x][y - 1].dielectricValue);
		
		oldPotentialField[lcenter.x][y] = potentialField[lcenter.x][y];
		bufferPotentialField[lcenter.x][y] = potentialField[lcenter.x][y];
	}
	for (int y = lcenter.y - 1; y > leftBottom.y; y--)
	{
		potentialField[lcenter.x][y] = computeCellPotential(
			oldPotentialField[lcenter.x][y],
			potentialField[lcenter.x - 1][y],
			potentialField[lcenter.x][y + 1],
			potentialField[lcenter.x + 1][y],
			potentialField[lcenter.x][y - 1],
			matrix[lcenter.x - 1][y].dielectricValue,
			matrix[lcenter.x][y + 1].dielectricValue,
			matrix[lcenter.x + 1][y].dielectricValue,
			matrix[lcenter.x][y - 1].dielectricValue);

		oldPotentialField[lcenter.x][y] = potentialField[lcenter.x][y];
		bufferPotentialField[lcenter.x][y] = potentialField[lcenter.x][y];
	}

	// compute top horizontal edge
	for (int x = tcenter.x + 1; x < rightTop.x; x++)
	{
		potentialField[x][tcenter.y] = computeCellPotential(
			oldPotentialField[x][tcenter.y],
			potentialField[x - 1][tcenter.y],
			potentialField[x][tcenter.y + 1],
			potentialField[x + 1][tcenter.y],
			potentialField[x][tcenter.y - 1],
			matrix[x - 1][tcenter.y].dielectricValue,
			matrix[x][tcenter.y + 1].dielectricValue,
			matrix[x + 1][tcenter.y].dielectricValue,
			matrix[x][tcenter.y - 1].dielectricValue);

		oldPotentialField[x][tcenter.y] = potentialField[x][tcenter.y];
		bufferPotentialField[x][tcenter.y] = potentialField[x][tcenter.y];
	}
	for (int x = tcenter.x - 1; x > leftTop.x; x--)
	{
		potentialField[x][tcenter.y] = computeCellPotential(
			oldPotentialField[x][tcenter.y],
			potentialField[x - 1][tcenter.y],
			potentialField[x][tcenter.y + 1],
			potentialField[x + 1][tcenter.y],
			potentialField[x][tcenter.y - 1],
			matrix[x - 1][tcenter.y].dielectricValue,
			matrix[x][tcenter.y + 1].dielectricValue,
			matrix[x + 1][tcenter.y].dielectricValue,
			matrix[x][tcenter.y - 1].dielectricValue);

		oldPotentialField[x][tcenter.y] = potentialField[x][tcenter.y];
		bufferPotentialField[x][tcenter.y] = potentialField[x][tcenter.y];
	}

	//compute right vertical edge
	for (int y = rcenter.y + 1; y < rightTop.y; y++)
	{
		potentialField[rcenter.x][y] = computeCellPotential(
			oldPotentialField[rcenter.x][y],
			potentialField[rcenter.x - 1][y],
			potentialField[rcenter.x][y + 1],
			potentialField[rcenter.x + 1][y],
			potentialField[rcenter.x][y - 1],
			matrix[rcenter.x - 1][y].dielectricValue,
			matrix[rcenter.x][y + 1].dielectricValue,
			matrix[rcenter.x + 1][y].dielectricValue,
			matrix[rcenter.x][y - 1].dielectricValue);

		oldPotentialField[rcenter.x][y] = potentialField[rcenter.x][y];
		bufferPotentialField[rcenter.x][y] = potentialField[rcenter.x][y];
	}
	for (int y = rcenter.y - 1; y > rightBottom.y; y--)
	{
		potentialField[rcenter.x][y] = computeCellPotential(
			oldPotentialField[rcenter.x][y],
			potentialField[rcenter.x - 1][y],
			potentialField[rcenter.x][y + 1],
			potentialField[rcenter.x + 1][y],
			potentialField[rcenter.x][y - 1],
			matrix[rcenter.x - 1][y].dielectricValue,
			matrix[rcenter.x][y + 1].dielectricValue,
			matrix[rcenter.x + 1][y].dielectricValue,
			matrix[rcenter.x][y - 1].dielectricValue);

		oldPotentialField[rcenter.x][y] = potentialField[rcenter.x][y];
		bufferPotentialField[rcenter.x][y] = potentialField[rcenter.x][y];
	}

	// compute bottom horizontal edge
	for (int x = bcenter.x + 1; x < rightBottom.x; x++)
	{
		potentialField[x][bcenter.y] = computeCellPotential(
			oldPotentialField[x][bcenter.y],
			potentialField[x - 1][bcenter.y],
			potentialField[x][bcenter.y + 1],
			potentialField[x + 1][bcenter.y],
			potentialField[x][bcenter.y - 1],
			matrix[x - 1][bcenter.y].dielectricValue,
			matrix[x][bcenter.y + 1].dielectricValue,
			matrix[x + 1][bcenter.y].dielectricValue,
			matrix[x][bcenter.y - 1].dielectricValue);

		oldPotentialField[x][bcenter.y] = potentialField[x][bcenter.y];
		bufferPotentialField[x][bcenter.y] = potentialField[x][bcenter.y];
	}
	for (int x = tcenter.x - 1; x > leftTop.x; x--)
	{
		potentialField[x][bcenter.y] = computeCellPotential(
			oldPotentialField[x][bcenter.y],
			potentialField[x - 1][bcenter.y],
			potentialField[x][bcenter.y + 1],
			potentialField[x + 1][bcenter.y],
			potentialField[x][bcenter.y - 1],
			matrix[x - 1][bcenter.y].dielectricValue,
			matrix[x][bcenter.y + 1].dielectricValue,
			matrix[x + 1][bcenter.y].dielectricValue,
			matrix[x][bcenter.y - 1].dielectricValue);

		oldPotentialField[x][bcenter.y] = potentialField[x][bcenter.y];
		bufferPotentialField[x][bcenter.y] = potentialField[x][bcenter.y];
	}

	// compute corner points on edges
	potentialField[leftTop.x][leftTop.y] = computeCellPotential(
		oldPotentialField[leftTop.x][leftTop.y],
		potentialField[leftTop.x - 1][leftTop.y],
		potentialField[leftTop.x][leftTop.y + 1],
		potentialField[leftTop.x + 1][leftTop.y],
		potentialField[leftTop.x][leftTop.y - 1],
		matrix[leftTop.x - 1][leftTop.y].dielectricValue,
		matrix[leftTop.x][leftTop.y + 1].dielectricValue,
		matrix[leftTop.x + 1][leftTop.y].dielectricValue,
		matrix[leftTop.x][leftTop.y - 1].dielectricValue);
	
	potentialField[rightTop.x][rightTop.y] = computeCellPotential(
		oldPotentialField[rightTop.x][rightTop.y],
		potentialField[rightTop.x - 1][rightTop.y],
		potentialField[rightTop.x][rightTop.y + 1],
		potentialField[rightTop.x + 1][rightTop.y],
		potentialField[rightTop.x][rightTop.y - 1],
		matrix[rightTop.x - 1][rightTop.y].dielectricValue,
		matrix[rightTop.x][rightTop.y + 1].dielectricValue,
		matrix[rightTop.x + 1][rightTop.y].dielectricValue,
		matrix[rightTop.x][rightTop.y - 1].dielectricValue);

	potentialField[rightBottom.x][rightBottom.y] = computeCellPotential(
		oldPotentialField[rightBottom.x][rightBottom.y],
		potentialField[rightBottom.x - 1][rightBottom.y],
		potentialField[rightBottom.x][rightBottom.y + 1],
		potentialField[rightBottom.x + 1][rightBottom.y],
		potentialField[rightBottom.x][rightBottom.y - 1],
		matrix[rightBottom.x - 1][rightBottom.y].dielectricValue,
		matrix[rightBottom.x][rightBottom.y + 1].dielectricValue,
		matrix[rightBottom.x + 1][rightBottom.y].dielectricValue,
		matrix[rightBottom.x][rightBottom.y - 1].dielectricValue);

	potentialField[leftBottom.x][leftBottom.y] = computeCellPotential(
		oldPotentialField[leftBottom.x][leftBottom.y],
		potentialField[leftBottom.x - 1][leftBottom.y],
		potentialField[leftBottom.x][leftBottom.y + 1],
		potentialField[leftBottom.x + 1][leftBottom.y],
		potentialField[leftBottom.x][leftBottom.y - 1],
		matrix[leftBottom.x - 1][leftBottom.y].dielectricValue,
		matrix[leftBottom.x][leftBottom.y + 1].dielectricValue,
		matrix[leftBottom.x + 1][leftBottom.y].dielectricValue,
		matrix[leftBottom.x][leftBottom.y - 1].dielectricValue);

	oldPotentialField[leftTop.x][leftTop.y] = bufferPotentialField[leftTop.x][leftTop.y];
	oldPotentialField[rightTop.x][rightTop.y] = bufferPotentialField[rightTop.x][rightTop.y];
	oldPotentialField[rightBottom.x][rightBottom.y] = bufferPotentialField[rightBottom.x][rightBottom.y];
	oldPotentialField[leftBottom.x][leftBottom.y] = bufferPotentialField[leftBottom.x][leftBottom.y];

	bufferPotentialField[leftTop.x][leftTop.y] = potentialField[leftTop.x][leftTop.y];
	bufferPotentialField[rightTop.x][rightTop.y] = potentialField[rightTop.x][rightTop.y];
	bufferPotentialField[rightBottom.x][rightBottom.y] = potentialField[rightBottom.x][rightBottom.y];
	bufferPotentialField[leftBottom.x][leftBottom.y] = potentialField[leftBottom.x][leftBottom.y];
}



// Compute potential field
void GridSolver::computeFieldPotential(
	const Matrix2D<Types::CellInfo>& matrix, 
	Matrix2D<double>& potentialField, 
	const Vector<Point2D<int>>& initCells) const
{
	int rows = matrix.getRows() - 1;
	int cols = matrix.getCols() - 1;

	Matrix2D<double> oldPotentialField(rows, cols);

	Matrix2D<double> bufferPotentialField(rows, cols);

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			oldPotentialField[y][x] = potentialField[y][x];
			bufferPotentialField[y][x] = potentialField[y][x];
		}
	}

	Vector<bool> isPropagationComplited(initCells.getLength());

	double currentAccuracy = 0.0;

	while (currentAccuracy >= accuracy)
	{
		currentAccuracy = 1.0;

		int offsetPropagationRect = 0;

		bool isAllPropagationComplited = false;

		for (int i = 0; i < isPropagationComplited.getLength(); i++)
		{
			isPropagationComplited[i] = false;
		}

		while (!isAllPropagationComplited)
		{
			offsetPropagationRect++;

			for (int i = 0; i < initCells.getLength(); i++)
			{
				if (isPropagationComplited[i] == false)
				{
					int l = initCells[i].x;
					int t = initCells[i].y;
					int r = initCells[i].x;
					int b = initCells[i].y;

					if (initCells[i].x - offsetPropagationRect > 0) l -= offsetPropagationRect;
					else l = 0;

					if (initCells[i].y + offsetPropagationRect < rows) t += offsetPropagationRect;
					else t = rows;

					if (initCells[i].x + offsetPropagationRect < cols) r += offsetPropagationRect;
					else r = cols;

					if (initCells[i].y - offsetPropagationRect > 0) b -= offsetPropagationRect;
					else b = 0;

					if (l == 0 && t == rows && r == cols && b == 0)
					{
						isPropagationComplited[i] = true;
					}
					else
					{
						computeRectPropagation(
							matrix,
							oldPotentialField,
							bufferPotentialField,
							potentialField,
							Point2D<int>(initCells[i].x, initCells[i].y),
							Rect2D<int>(l, t, r, b));
					}
				}
			}

			isAllPropagationComplited = true;
			for (int i = 0; i < isPropagationComplited.getLength(); i++)
			{
				if (isPropagationComplited[i] == false)
				{
					isAllPropagationComplited = false;
					break;
				}
			}
		}

		// compute min accuracy
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				if (matrix[y][x].isConductor == false && potentialField[y][x] != 0.0)
				{
					double tempAccuracy = abs(oldPotentialField[y][x] / potentialField[y][x]);
					if (currentAccuracy > tempAccuracy)
					{
						currentAccuracy = tempAccuracy;
					}
				}
			}
		}
	}
}


// Compute capacity for potentialField
double GridSolver::computeCapacity(const Matrix2D<Types::CellInfo>& matrix, const Matrix2D<double>& potentialField) const
{
	return 0.0;
}
