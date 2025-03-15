#include "GridSolver.h"



// Compute linear parameters of strip structure
// matrix - matrix of rasterized strip structure
Matrix2D<Types::LinearParameters> GridSolver::computeLinearParameters(const Matrix2D<Types::CellInfo>& structureMatrix) const
{
	auto start = std::chrono::steady_clock::now();

	// create copy matrix for some changes
	Matrix2D<Types::CellInfo> matrix(structureMatrix);

	// defines
	Vector<Vector<Point2D<int>>> condCells = defineAllConductorsCells(matrix);

	Point2D<int> verticalSymmetryPoint = defineVerticalSymmetryPoint(matrix);
	Point2D<int> horizontalSymmetryPoint = defineHorizontalSymmetryPoint(matrix);
	Vector<Point2D<int>> verticalSymmetryConductors = defineVerticalSymmetyConductors(condCells, verticalSymmetryPoint);
	Vector<Point2D<int>> horizontalSymmetryConductors = defineHorizontalSymmetyConductors(condCells, horizontalSymmetryPoint);
	Vector<Point2D<int>> symmetryConductors;
	for (int i = 0; i < verticalSymmetryConductors.getLength(); i++)
	{
		symmetryConductors.add(verticalSymmetryConductors[i]);
	}
	for (int i = 0; i < horizontalSymmetryConductors.getLength(); i++)
	{
		symmetryConductors.add(horizontalSymmetryConductors[i]);
	}
	Vector<Point2D<int>> initCells = defineInitialCellsForFieldPropagation(matrix, condCells);

	// compute linear capacity matrix for dielectric and air fill
	Matrix2D<Types::LinearParameters> linearParam(condCells.getLength(), condCells.getLength());

	computeLinearCapacityAndConductanceMatrix(
		matrix,
		condCells,
		initCells,
		symmetryConductors,
		linearParam,
		false);

	computeLinearCapacityAndConductanceMatrix(
		matrix,
		condCells,
		initCells,
		symmetryConductors,
		linearParam,
		true);

	if (Tool::lossless == false)
	{
		computeConductorLossMatrix(
			matrix,
			condCells,
			initCells,
			symmetryConductors,
			linearParam);
	}

	// primary parameters
	Matrix2D<Types::PrimaryParameters> primaryParameters(condCells.getLength(), condCells.getLength());

	Vector<long double> tempC(primaryParameters.getRows());
	Vector<long double> tempCAir(primaryParameters.getRows());
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		tempC[i] = 0.0;
		tempCAir[i] = 0.0;
		for (int j = i + 1; j < linearParam.getCols(); j++)
		{
			tempC[i] += linearParam[i][j].C;
			tempCAir[i] += linearParam[i][j].CAir;
		}
		for (int j = i - 1; j >= 0; j--)
		{
			tempC[i] += linearParam[i][j].C;
			tempCAir[i] += linearParam[i][j].CAir;
		}
	}
	for (int i = 0; i < primaryParameters.getRows(); i++)
	{
		primaryParameters[i][i].C1 = linearParam[i][i].C + tempC[i];
		primaryParameters[i][i].C1Air = linearParam[i][i].CAir + tempCAir[i];

		for (int j = i + 1; j < primaryParameters.getCols(); j++)
		{
			primaryParameters[i][j].C1 = -linearParam[i][j].C;
			primaryParameters[i][j].C1Air = -linearParam[i][j].CAir;
			primaryParameters[j][i].C1 = -linearParam[i][j].C;
			primaryParameters[j][i].C1Air = -linearParam[i][j].CAir;
		}
	}


	if (primaryParameters.getRows() == 1)
	{
		primaryParameters[0][0].epsEff = linearParam[0][0].C / linearParam[0][0].CAir;
		primaryParameters[0][0].Z1 = sqrt(linearParam[0][0].L / linearParam[0][0].C);
		primaryParameters[0][0].Z0 = primaryParameters[0][0].Z1;
	}
	else if (primaryParameters.getRows() == 2)
	{
		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			for (int j = i + 1; j < primaryParameters.getCols(); j++)
			{
				primaryParameters[i][i].epsEff = Types::speedLight * Types::speedLight * (linearParam[i][i].C + linearParam[i][j].C) * (linearParam[i][i].L + linearParam[i][j].L);
				primaryParameters[i][j].epsEff = Types::speedLight * Types::speedLight * (linearParam[i][i].C - linearParam[i][j].C) * (linearParam[i][i].L - linearParam[i][j].L);
			}
			for (int j = i - 1; j >= 0; j--)
			{
				primaryParameters[i][i].epsEff = Types::speedLight * Types::speedLight * (linearParam[i][i].C + linearParam[i][j].C) * (linearParam[i][i].L + linearParam[i][j].L);
				primaryParameters[i][j].epsEff = Types::speedLight * Types::speedLight * (linearParam[i][i].C - linearParam[i][j].C) * (linearParam[i][i].L - linearParam[i][j].L);
			}
		}

		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			for (int j = i + 1; j < primaryParameters.getCols(); j++)
			{
				primaryParameters[i][i].Z1 = sqrt((linearParam[i][i].L + linearParam[i][j].L) / (linearParam[i][i].C + linearParam[i][j].C));
				primaryParameters[i][j].Z1 = sqrt((linearParam[i][i].L - linearParam[i][j].L) / (linearParam[i][i].C - linearParam[i][j].C));
			}
			for (int j = i - 1; j >= 0; j--)
			{
				primaryParameters[i][i].Z1 = sqrt((linearParam[i][i].L + linearParam[i][j].L) / (linearParam[i][i].C + linearParam[i][j].C));
				primaryParameters[i][j].Z1 = sqrt((linearParam[i][i].L - linearParam[i][j].L) / (linearParam[i][i].C - linearParam[i][j].C));
			}
		}

		Vector<long double> tempZ(primaryParameters.getRows());
		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			tempZ[i] = 1.0;
			for (int j = 0; j < primaryParameters.getCols(); j++)
			{
				tempZ[i] *= primaryParameters[i][j].Z1;
			}
		}
		for (int i = 0; i < tempZ.getLength(); i++)
		{
			primaryParameters[0][0].Z0 += tempZ[i];
		}
		primaryParameters[0][0].Z0 /= tempZ.getLength();
		primaryParameters[0][0].Z0 = sqrt(primaryParameters[0][0].Z0);
	}
	else
	{
		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			for (int j = 0; j < primaryParameters.getCols(); j++)
			{
				primaryParameters[i][j].epsEff = primaryParameters[i][j].C1 / primaryParameters[i][j].C1Air;
			}
		}
	}

	auto end = std::chrono::steady_clock::now();
	double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "\n\n";
	std::cout << "**********************************************************************************************************************************************************************\n";
	std::cout << "* > RESULTS" << "\n";
	std::cout << "* > Total calculation time of the grid method: " << time / 1000 << " seconds\n";
	std::cout << "**********************************************************************************************************************************************************************\n\n";

	printResultInfo(
		verticalSymmetryPoint,
		horizontalSymmetryPoint,
		verticalSymmetryConductors,
		horizontalSymmetryConductors,
		symmetryConductors,
		linearParam,
		primaryParameters);

	return linearParam;
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



// Check structure on symmetry
// return symmetry point: left and right X coordinate - Point(leftX ; rightX)
// left and right coordinates may be equal
// return Point(0 ; 0) if structure has no symmetry
Point2D<int> GridSolver::defineVerticalSymmetryPoint(const Matrix2D<Types::CellInfo>& matrix) const
{
	// check vertical symmetry
	int rows = matrix.getRows();
	int cols = matrix.getCols();

	Point2D<int> centerPoint = Point2D<int>(0, 0);

	// Added an error counter.
	// This is related to sampling and rasterization problems.
	// If only one wrong column is encountered when defining symmetry - this structure is still considered symmetric
	bool isSymmentry = true;
	bool foundNonSymmetry = false;
	int error1 = 0;
	int globalError1 = 0;
	int center = cols / 2;

	for (int x = center - 1; x >= 1; x--)
	{
		int offset = center - x;
		for (int y = 1; y < matrix.getRows(); y++)
		{
			if (matrix[y][x] != matrix[y][center + offset])
			{
				error1++;
				globalError1++;
				foundNonSymmetry = true;
				break;
			}
			else
			{
				foundNonSymmetry = false;
			}
		}
		if (foundNonSymmetry == false)
		{
			error1 = 0;
		}
		else if (error1 > 1)
		{
			isSymmentry = false;
			break;
		}
	}
	if (isSymmentry == true)
	{
		centerPoint = Point2D(center, center);
	}

	isSymmentry = true;
	foundNonSymmetry = false;
	int error2 = 0;
	int globalError2 = 0;
	center = cols / 2 - 1;

	for (int x = center - 1; x >= 1; x--)
	{
		int offset = center - x;
		for (int y = 1; y < matrix.getRows(); y++)
		{
			if (matrix[y][x] != matrix[y][center + offset])
			{
				error2++;
				globalError2++;
				foundNonSymmetry = true;
				break;
			}
			else
			{
				foundNonSymmetry = false;
			}
		}
		if (foundNonSymmetry == false)
		{
			error2 = 0;
		}
		else if (error2 > 1)
		{
			isSymmentry = false;
			break;
		}
	}
	if (isSymmentry == true && globalError2 < globalError1)
	{
		centerPoint = Point2D(center, center);
	}

	isSymmentry = true;
	foundNonSymmetry = false;
	int error3 = 0;
	int globalError3 = 0;
	int rightCenter = (double)cols / 2.0;
	int leftCenter = rightCenter - 1;

	for (int x = leftCenter - 1; x >= 1; x--)
	{
		int offset = leftCenter - x;
		for (int y = 1; y < matrix.getRows(); y++)
		{
			if (matrix[y][x] != matrix[y][rightCenter + offset])
			{
				error3++;
				globalError3++;
				foundNonSymmetry = true;
				break;
			}
			else
			{
				foundNonSymmetry = false;
			}
		}
		if (foundNonSymmetry == false)
		{
			error3 = 0;
		}
		else if (error3 > 1)
		{
			isSymmentry = false;
			break;
		}
	}

	if (isSymmentry == true && globalError3 < globalError2)
	{
		centerPoint = Point2D(leftCenter, rightCenter);
	}

	isSymmentry = true;
	foundNonSymmetry = false;
	int error4 = 0;
	int globalError4 = 0;
	rightCenter = (double)cols / 2.0 + 1.0;
	leftCenter = rightCenter - 1;

	for (int x = leftCenter - 1; x >= 1; x--)
	{
		int offset = leftCenter - x;
		for (int y = 1; y < matrix.getRows(); y++)
		{
			if (matrix[y][x] != matrix[y][rightCenter + offset])
			{
				error4++;
				globalError4++;
				foundNonSymmetry = true;
				break;
			}
			else
			{
				foundNonSymmetry = false;
			}
		}
		if (foundNonSymmetry == false)
		{
			error4 = 0;
		}
		else if (error4 > 1)
		{
			isSymmentry = false;
			break;
		}
	}

	if (isSymmentry == true && globalError4 < globalError3)
	{
		centerPoint = Point2D(leftCenter, rightCenter);
	}

	return centerPoint;
}



// Check structure on symmetry
// return symmetry point: bottom and top Y coordinate - Point(bottomX ; topY)
// bottom and top coordinates may be equal
// return Point(0 ; 0) if structure has no symmetry
Point2D<int> GridSolver::defineHorizontalSymmetryPoint(const Matrix2D<Types::CellInfo>& matrix) const
{
	// check vertical symmetry
	int rows = matrix.getRows();
	int cols = matrix.getCols();

	// Added an error counter.
	// This is related to sampling and rasterization problems.
	// If only one wrong column is encountered when defining symmetry - this structure is still considered symmetric
	bool isSymmentry = true;
	bool foundNonSymmetry = false;
	int error = 0;
	int center = rows / 2 - 1;

	for (int y = center - 1; y >= 1; y--)
	{
		int offset = center - y;
		for (int x = 1; x < matrix.getCols(); x++)
		{
			if (matrix[y][x] != matrix[center + offset][x])
			{
				error++;
				foundNonSymmetry = true;
				break;
			}
			else
			{
				foundNonSymmetry = false;
			}
		}
		if (foundNonSymmetry == false)
		{
			error = 0;
		}
		else if (error > 1)
		{
			isSymmentry = false;
			break;
		}
	}

	if (isSymmentry == true)
	{
		return Point2D(center, center);
	}

	isSymmentry = true;
	foundNonSymmetry = false;
	error = 0;
	int topCenter = (double)rows / 2.0;
	int bottomCenter = topCenter - 1;

	for (int y = bottomCenter - 1; y >= 1; y--)
	{
		int offset = bottomCenter - y;
		for (int x = 1; x < matrix.getCols(); x++)
		{
			if (matrix[y][x] != matrix[topCenter + offset][x])
			{
				error++;
				foundNonSymmetry = true;
				break;
			}
			else
			{
				foundNonSymmetry = false;
			}
		}
		if (foundNonSymmetry == false)
		{
			error = 0;
		}
		else if (error > 1)
		{
			isSymmentry = false;
			break;
		}
	}

	if (isSymmentry == true)
	{
		return Point2D(bottomCenter, topCenter);
	}

	return Point2D(0, 0);
}



// Define pair of symmetrycal conductors
// return point: first and numbers - numbers of symmetrical conductors
// return empty Vector if there is no symmetry
Vector<Point2D<int>> GridSolver::defineVerticalSymmetyConductors(
	const Vector<Vector<Point2D<int>>>& conductorsCells,
	const Point2D<int>& symmetryPoint) const
{
	Vector<Point2D<int>> symmetryConductrors;

	if (symmetryPoint == Point2D<int>(0, 0))
	{
		return symmetryConductrors;
	}

	for (int i = 0; i < conductorsCells.getLength() - 1; i++)
	{
		for (int j = i + 1; j < conductorsCells.getLength(); j++)
		{
			bool isSymmetry = true;

			// A small margin of error has been added.
			// One wire may be one cell longer than the other.
			// If they are otherwise identical, they are counted symmetrically
			int minCondLength = conductorsCells[i].getLength();
			int maxCondLength = conductorsCells[j].getLength();
			if (minCondLength > maxCondLength)
			{
				int bufferLength = maxCondLength;
				maxCondLength = minCondLength;
				minCondLength = bufferLength;
			}

			if (minCondLength == maxCondLength || minCondLength == maxCondLength - 1)
			{
				for (int k = 0; k < minCondLength; k++)
				{
					int leftPoint = minCondLength - 1 - k;
					int rightPoint = k;
					Point2D<int> firstCondPoint = conductorsCells[i][leftPoint];
					Point2D<int> secondCondPoint = conductorsCells[j][rightPoint];
					firstCondPoint.x = abs(symmetryPoint.x - firstCondPoint.x);
					secondCondPoint.x = abs(symmetryPoint.y - secondCondPoint.x);
					if (firstCondPoint != secondCondPoint)
					{
						isSymmetry = false;
						break;
					}
				}
			}
			else
			{
				isSymmetry = false;
			}

			if (isSymmetry == true)
			{
				symmetryConductrors.add(Point2D<int>(i, j));
			}
		}
	}

	return symmetryConductrors;
}



// Define pair of symmetrycal conductors
// return point: first and numbers - numbers of symmetrical conductors
// return Point2D(-1 ; -1) if there is no symmetry
Vector<Point2D<int>> GridSolver::defineHorizontalSymmetyConductors(
	const Vector<Vector<Point2D<int>>>& conductorsCells,
	const Point2D<int>& symmetryPoint) const
{
	Vector<Point2D<int>> symmetryConductrors;

	if (symmetryPoint == Point2D<int>(0, 0))
	{
		return symmetryConductrors;
	}

	for (int i = 0; i < conductorsCells.getLength() - 1; i++)
	{
		for (int j = i + 1; j < conductorsCells.getLength(); j++)
		{
			bool isSymmetry = true;

			// A small margin of error has been added.
			// One wire may be one cell longer than the other.
			// If they are otherwise identical, they are counted symmetrically
			int minCondLength = conductorsCells[i].getLength();
			int maxCondLength = conductorsCells[j].getLength();
			if (minCondLength > maxCondLength)
			{
				int bufferLength = maxCondLength;
				maxCondLength = minCondLength;
				minCondLength = bufferLength;
			}

			if (minCondLength == maxCondLength || minCondLength == maxCondLength - 1)
			{
				for (int k = 0; k < minCondLength; k++)
				{
					int bottomPoint = k;
					int topPoint = k;
					Point2D<int> firstCondPoint = conductorsCells[i][bottomPoint];
					Point2D<int> secondCondPoint = conductorsCells[j][topPoint];
					firstCondPoint.y = abs(symmetryPoint.x - firstCondPoint.y);
					secondCondPoint.y = abs(symmetryPoint.y - secondCondPoint.y);
					if (firstCondPoint != secondCondPoint)
					{
						isSymmetry = false;
						break;
					}
				}
			}
			else
			{
				isSymmetry = false;
			}

			if (isSymmetry == true)
			{
				symmetryConductrors.add(Point2D<int>(i, j));
			}
		}
	}

	return symmetryConductrors;
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
	Matrix2D<double>& bufferPotentialField,
	Matrix2D<double>& potentialField,
	const Point2D<int>& computedPoint) const
{
	int rows = potentialField.getRows() - 1;
	int cols = potentialField.getCols() - 1;
	int x = computedPoint.x;
	int y = computedPoint.y;


	//if (x > 0 && x < cols &&
	//	y > 0 && y < rows &&
	//	potentialField[y][x] == bufferPotentialField[y][x])


	if (x > 0 && x < cols &&
		y > 0 && y < rows)
	{
		if (matrix[y][x].isConductor == false)
		{
			double lu = potentialField[y][x - 1];
			double tu = potentialField[y + 1][x];
			double ru = potentialField[y][x + 1];
			double bu = potentialField[y - 1][x];

			double e = matrix[y][x].dielectricValue;
			double le = matrix[y][x - 1].dielectricValue;
			double te = matrix[y + 1][x].dielectricValue;
			double re = matrix[y][x + 1].dielectricValue;
			double be = matrix[y - 1][x].dielectricValue;

			lu = (matrix[y][x - 1].dy * lu);
			tu = (matrix[y + 1][x].dx * tu);
			ru = (matrix[y][x + 1].dy * ru);
			bu = (matrix[y - 1][x].dx * bu);

			
			double u = tu + bu + lu + ru;
			u = u / (2 * matrix[y][x].dx + 2 * matrix[y][x].dy);

			if ((e != be || e != te) &&
				matrix[y - 1][x].isConductor == false &&
				matrix[y + 1][x].isConductor == false)
			{
				double sumE = (be + te) / 2;
				u = (te * tu) + (be * bu) + sumE * (lu + ru);
				u = u / ((2 * matrix[y][x].dx + 2 * matrix[y][x].dy) * sumE);
			}

			if ((e != le || e != re) &&
				matrix[y][x - 1].isConductor == false &&
				matrix[y][x + 1].isConductor == false)
			{
				double sumE = (re + le) / 2;
				u = sumE * (tu + bu) + (le * lu) + (re * ru);
				u = u / ((2 * matrix[y][x].dx + 2 * matrix[y][x].dy) * sumE);
			}

			/*
			if ((e != be || e != te) &&
				matrix[y - 1][x].isConductor == false &&
				matrix[y + 1][x].isConductor == false)
			{
				u = (2 * te * tu) / (te + be) + (2 * be * bu) / (te + be) + lu + ru;
				u = u / (2 * matrix[y][x].dx + 2 * matrix[y][x].dy);
				matrix[y][x].isHorizontalDielectricBound = true;
			}

			if ((e != le || e != re) &&
				matrix[y][x - 1].isConductor == false &&
				matrix[y][x + 1].isConductor == false)
			{
				u = tu + bu + (2 * le * lu) / (le + re) + (2 * re * ru) / (le + re);
				u = u / (2 * matrix[y][x].dx + 2 * matrix[y][x].dy);
				matrix[y][x].isVerticalDielectricBound = true;
			}
			*/
			/*
			if (le == re && (e != be || e != te))
			{
				if (matrix[y - 1][x].isHorizontalDielectricBound == false &&
					matrix[y + 1][x].isHorizontalDielectricBound == false &&
					matrix[y - 1][x].isConductor == false &&
					matrix[y + 1][x].isConductor == false)
				{
					double sumE = (be + te) / 2;
					u = (te * tu) + (be * bu) + sumE * (lu + ru);
					u = u / ((2 * matrix[y][x].dx + 2 * matrix[y][x].dy) * sumE);
					matrix[y][x].isHorizontalDielectricBound = true;
				}
			}
			
			else if (te == be && (e != le || e != re))
			{
				if (matrix[y][x - 1].isVerticalDielectricBound == false &&
					matrix[y][x + 1].isVerticalDielectricBound == false &&
					matrix[y][x - 1].isConductor == false &&
					matrix[y][x + 1].isConductor == false)
				{
					double sumE = (re + le) / 2;
					u = sumE * (tu + bu) + (le * lu) + (re * ru);
					u = u / ((2 * matrix[y][x].dx + 2 * matrix[y][x].dy) * sumE);
					matrix[y][x].isVerticalDielectricBound = true;
				}
			}
			*/

			potentialField[y][x] = relaxationCoeff * u + (1 - relaxationCoeff) * potentialField[y][x];
		}
	}
}



// Compute all cells around the edges of rect propagation
void GridSolver::computeRectPropagation(
	const Matrix2D<Types::CellInfo>& matrix,
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
	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		lcenter);

	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		tcenter);

	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		rcenter);

	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		bcenter);
	
	// compute left vertical edge
	for (int y = lcenter.y + 1; y < leftTop.y; y++)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(lcenter.x, y));
	}
	for (int y = lcenter.y - 1; y > leftBottom.y; y--)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(lcenter.x, y));
	}

	// compute top horizontal edge
	for (int x = tcenter.x + 1; x < rightTop.x; x++)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(x, tcenter.y));
	}
	for (int x = tcenter.x - 1; x > leftTop.x; x--)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(x, tcenter.y));
	}

	//compute right vertical edge
	for (int y = rcenter.y + 1; y < rightTop.y; y++)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(rcenter.x, y));
	}
	for (int y = rcenter.y - 1; y > rightBottom.y; y--)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(rcenter.x, y));
	}

	// compute bottom horizontal edge
	for (int x = bcenter.x + 1; x < rightBottom.x; x++)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(x, bcenter.y));
	}
	for (int x = tcenter.x - 1; x > leftTop.x; x--)
	{
		computeCellPotential(
			matrix,
			bufferPotentialField,
			potentialField,
			Point2D<int>(x, bcenter.y));
	}

	// compute corner points on edges
	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		leftTop);

	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		rightTop);

	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		rightBottom);

	computeCellPotential(
		matrix,
		bufferPotentialField,
		potentialField,
		leftBottom);
}



// Compute potential field
int GridSolver::computeFieldPotential(
	const Matrix2D<Types::CellInfo>& matrix, 
	Matrix2D<double>& potentialField, 
	const Vector<Point2D<int>>& initCells) const
{
	int rows = matrix.getRows() - 1;
	int cols = matrix.getCols() - 1;

	Vector<bool> isPropagationComplited(initCells.getLength());

	Matrix2D<double> bufferPotentialField(rows, cols);

	double currentAccuracy = 0.0;
	int iteration = 0;

	for (int y = 1; y < rows; y++)
	{
		for (int x = 1; x < cols; x++)
		{
			matrix[y][x].isHorizontalDielectricBound = false;
			matrix[y][x].isVerticalDielectricBound = false;
		}
	}

	while (currentAccuracy <= accuracy)
	{
		int offsetPropagationRect = 0;
		int leftPropagation = 0;
		int topPropagation = 0;
		int rightPropagation = 0;
		int bottomPropagation = 0;

		bool isAllPropagationComplited = false;

		for (int i = 0; i < isPropagationComplited.getLength(); i++)
		{
			isPropagationComplited[i] = false;
		}

		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				bufferPotentialField[y][x] = potentialField[y][x];
			}
		}

		while (!isAllPropagationComplited)
		{
			offsetPropagationRect++;

			for (int i = 0; i < initCells.getLength(); i++)
			{
				if (potentialField[initCells[i].y][initCells[i].x] == 0.0)
				{
					isPropagationComplited[i] = true;
				}

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

		// compute min current accuracy
		double minTempAccuracy = 1.0;

		for (int y = 1; y < rows; y++)
		{
			for (int x = 1; x < cols; x++)
			{
				if (matrix[y][x].isConductor == false && potentialField[y][x] != 0.0)
				{
					double bufferPotential = bufferPotentialField[y][x];
					double potential = potentialField[y][x];
					double tempAccuracy = abs(bufferPotential / potential);
					if (minTempAccuracy > tempAccuracy)
					{
						minTempAccuracy = tempAccuracy;
					}
				}
			}
		}

		currentAccuracy = minTempAccuracy;
		iteration++;
	}

	return iteration;
}



// Compute capacity and conductance for potential field
// return point from to value: [capacity ; conductance]
Point2D<double> GridSolver::computeCapacityAndConductance(const Matrix2D<Types::CellInfo>&matrix, const Matrix2D<double>&potentialField) const
{
	int rows = matrix.getRows() - 1;
	int cols = matrix.getCols() - 1;

	double dkEnergy = 0.0;
	double dfEnergy = 0.0;

	for (int y = 1; y < rows; y++)
	{
		for (int x = 1; x < cols; x++)
		{
			if (matrix[y][x].isConductor == false)
			{
				double first = potentialField[y][x] - potentialField[y + 1][x + 1];
				double second = potentialField[y][x + 1] - potentialField[y + 1][x];
				first = first * first;
				second = second * second;

				dkEnergy += (matrix[y][x].dielectricValue / 4) * (first + second);
				dfEnergy += (matrix[y][x].dielectricValue * matrix[y][x].tgValue / 4) * (first + second);
			}
		}
	}

	double capacity = 2 * Types::e0 * dkEnergy;
	double conductance = 2 * Types::e0 * dfEnergy * 1e10;

	return Point2D<double>(capacity, conductance);
}



// Compute linear capacity matrix
void GridSolver::computeLinearCapacityAndConductanceMatrix(
	Matrix2D<Types::CellInfo>& matrix,
	const Vector<Vector<Point2D<int>>>& condCells,
	const Vector<Point2D<int>>& initCells,
	const Vector<Point2D<int>>& symmetryConductors,
	Matrix2D<Types::LinearParameters>& linearParam,
	bool isAirFill) const
{
	if (isAirFill)
	{
		// set air fill
		for (int y = 0; y < matrix.getRows(); y++)
		{
			for (int x = 0; x < matrix.getCols(); x++)
			{
				matrix[y][x].dielectricValue = 1.0;
			}
		}
	}

	// compute diagonal elements in linear capacity matrix (full capacity for each signal conductor)
	Vector<bool> conductorsConfig(condCells.getLength());

	for (int i = 0; i < condCells.getLength(); i++)
	{
		bool computeTisConfig = true;

		for (int j = 0; j < symmetryConductors.getLength(); j++)
		{
			if (symmetryConductors[j].y == i)
			{
				if (isAirFill)
				{
					linearParam[i][i].CAir = linearParam[symmetryConductors[j].x][j].CAir;
				}
				else
				{
					linearParam[i][i].C = linearParam[symmetryConductors[j].x][j].C;
					linearParam[i][i].G = linearParam[symmetryConductors[j].x][j].G;
				}
				computeTisConfig = false;
				break;
			}
		}

		if (computeTisConfig == true)
		{
			for (int j = 0; j < conductorsConfig.getLength(); j++)
			{
				conductorsConfig[j] = false;
			}

			conductorsConfig[i] = true;

			auto start = std::chrono::steady_clock::now();
			Matrix2D<double>potentialField = setupInitialPotential(matrix, condCells, conductorsConfig);
			int iterations = computeFieldPotential(matrix, potentialField, initCells);
			auto end = std::chrono::steady_clock::now();
			double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			if (isAirFill)
			{
				linearParam[i][i].CAir = computeCapacityAndConductance(matrix, potentialField).x;

				std::cout << "\n\n";
				std::cout << "**********************************************************************************************************************************************************************\n";
				std::cout << "* > Potential field computation for air fill has been completed" << "\n";
				std::cout << "* > Conductors config: " << conductorsConfig << "\n";
				std::cout << "* > The compute was done in " << iterations << " iterations\n";
				std::cout << "* > The compute was done in " << time / 1000 << " seconds\n";
				std::cout << "**********************************************************************************************************************************************************************\n";
			}
			else
			{
				linearParam[i][i].C = computeCapacityAndConductance(matrix, potentialField).x;
				linearParam[i][i].G = computeCapacityAndConductance(matrix, potentialField).y;

				std::cout << "\n\n\n";
				std::cout << "**********************************************************************************************************************************************************************\n";
				std::cout << "* > Potential field computation for dielectric fill has been completed" << "\n";
				std::cout << "* > Conductors config: " << conductorsConfig << "\n";
				std::cout << "* > The compute was done in " << iterations << " iterations\n";
				std::cout << "* > The compute was done in " << time / 1000 << " seconds\n";
				std::cout << "**********************************************************************************************************************************************************************\n";
			}

			if (Tool::drawField == true)
			{
				drawField(potentialField);
			}
		}
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
			bool computeTisConfig = true;

			for (int k = 0; k < symmetryConductors.getLength(); k++)
			{
				if (symmetryConductors[k].y == i)
				{
					if (isAirFill)
					{
						linearParam[i][j].CAir = linearParam[symmetryConductors[k].x][j].CAir;
						linearParam[j][i].CAir = linearParam[i][j].CAir;
					}
					else
					{
						linearParam[i][j].C = linearParam[symmetryConductors[k].x][j].C;
						linearParam[j][i].C = linearParam[i][j].C;

						linearParam[i][j].G = linearParam[symmetryConductors[k].x][j].G;
						linearParam[j][i].G = linearParam[i][j].G;
					}
					computeTisConfig = false;
					break;
				}
			}

			if (computeTisConfig == true)
			{
				conductorsConfig[j] = true;

				Matrix2D<double>potentialField = setupInitialPotential(matrix, condCells, conductorsConfig);

				auto start = std::chrono::steady_clock::now();
				int iterations = computeFieldPotential(matrix, potentialField, initCells);
				auto end = std::chrono::steady_clock::now();
				double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

				if (isAirFill)
				{
					linearParam[i][j].CAir = (computeCapacityAndConductance(matrix, potentialField).x - linearParam[i][i].CAir - linearParam[j][j].CAir) / 2.0;
					linearParam[j][i].CAir = linearParam[i][j].CAir;

					std::cout << "\n\n";
					std::cout << "**********************************************************************************************************************************************************************\n";
					std::cout << "* > Potential field computation for air fill has been completed" << "\n";
					std::cout << "* > Conductors config: " << conductorsConfig << "\n";
					std::cout << "* > The compute was done in " << iterations << " iterations\n";
					std::cout << "* > The compute was done in " << time / 1000 << " seconds\n";
					std::cout << "**********************************************************************************************************************************************************************\n";
				}
				else
				{
					linearParam[i][j].C = (computeCapacityAndConductance(matrix, potentialField).x - linearParam[i][i].C - linearParam[j][j].C) / 2.0;
					linearParam[j][i].C = linearParam[i][j].C;

					linearParam[i][j].G = (computeCapacityAndConductance(matrix, potentialField).y - linearParam[i][i].G - linearParam[j][j].G) / 2.0;
					linearParam[j][i].G = linearParam[i][j].G;

					std::cout << "\n\n\n";
					std::cout << "**********************************************************************************************************************************************************************\n";
					std::cout << "* > Potential field computation for dielectric fill has been completed" << "\n";
					std::cout << "* > Conductors config: " << conductorsConfig << "\n";
					std::cout << "* > The compute was done in " << iterations << " iterations\n";
					std::cout << "* > The compute was done in " << time / 1000 << " seconds\n";
					std::cout << "**********************************************************************************************************************************************************************\n";
				}

				if (Tool::drawField == true)
				{
					drawField(potentialField);
				}

				conductorsConfig[j] = false;
			}
		}
	}

	if (isAirFill == true)
	{
		// compute linear inductance
		Matrix2D<double> CAirMatrix(linearParam.getRows(), linearParam.getCols());
		for (int i = 0; i < CAirMatrix.getRows(); i++)
		{
			for (int j = 0; j < CAirMatrix.getCols(); j++)
			{
				CAirMatrix[i][j] = linearParam[i][j].CAir;
			}
		}

		if (CAirMatrix.getRows() == 1)
		{
			linearParam[0][0].L = Types::mu0 * Types::e0 * 1 / CAirMatrix[0][0];
		}
		else
		{
			Matrix2D<double> reverseCAirMatrix = MatrixTool::getReverseMatrix(CAirMatrix);
			for (int i = 0; i < linearParam.getRows(); i++)
			{
				for (int j = 0; j < linearParam.getCols(); j++)
				{
					linearParam[i][j].L = Types::mu0 * Types::e0 * reverseCAirMatrix[i][j];
				}
			}
		}
	}
}



// Compute conductor loss matrix
void GridSolver::computeConductorLossMatrix(
	Matrix2D<Types::CellInfo>& matrix,
	const Vector<Vector<Point2D<int>>>& condCells,
	const Vector<Point2D<int>>& initCells,
	const Vector<Point2D<int>>& symmetryConductors,
	Matrix2D<Types::LinearParameters>& linearParam) const
{
	// conductors extension
	Vector<Vector<Point2D<int>>> condCellsExtension(condCells);

	for (int i = 0; i < condCells.getLength(); i++)
	{
		for (int j = 0; j < condCells[i].getLength(); j++)
		{
			int x = condCells[i][j].x;
			int y = condCells[i][j].y;

			matrix[y + 1][x].isConductor = true;
			matrix[y + 1][x].isSignalConductor = true;

			matrix[y - 1][x].isConductor = true;
			matrix[y - 1][x].isSignalConductor = true;

			matrix[y][x + 1].isConductor = true;
			matrix[y][x + 1].isSignalConductor = true;

			matrix[y][x - 1].isConductor = true;
			matrix[y][x - 1].isSignalConductor = true;

			matrix[y + 1][x + 1].isConductor = true;
			matrix[y + 1][x + 1].isSignalConductor = true;

			matrix[y - 1][x - 1].isConductor = true;
			matrix[y - 1][x - 1].isSignalConductor = true;

			matrix[y - 1][x + 1].isConductor = true;
			matrix[y - 1][x + 1].isSignalConductor = true;

			matrix[y + 1][x - 1].isConductor = true;
			matrix[y + 1][x - 1].isSignalConductor = true;

			condCellsExtension[i].add(Point2D(x + 1, y));
			condCellsExtension[i].add(Point2D(x - 1, y));
			condCellsExtension[i].add(Point2D(x, y + 1));
			condCellsExtension[i].add(Point2D(x, y - 1));
			condCellsExtension[i].add(Point2D(x + 1, y + 1));
			condCellsExtension[i].add(Point2D(x - 1, y - 1));
			condCellsExtension[i].add(Point2D(x - 1, y + 1));
			condCellsExtension[i].add(Point2D(x + 1, y - 1));
		}
	}

	// set air fill
	for (int y = 0; y < matrix.getRows(); y++)
	{
		for (int x = 0; x < matrix.getCols(); x++)
		{
			matrix[y][x].dielectricValue = 1.0;
		}
	}

	// compute diagonal elements in extension linear capacity matrix for air fill (full capacity for each signal conductor)
	Vector<bool> conductorsConfig(condCells.getLength());

	for (int i = 0; i < condCells.getLength(); i++)
	{
		bool computeTisConfig = true;

		for (int j = 0; j < symmetryConductors.getLength(); j++)
		{
			if (symmetryConductors[j].y == i)
			{
				linearParam[i][i].CAirExtension = linearParam[symmetryConductors[j].x][j].CAirExtension;
				computeTisConfig = false;
				break;
			}
		}

		if (computeTisConfig == true)
		{
			for (int j = 0; j < conductorsConfig.getLength(); j++)
			{
				conductorsConfig[j] = false;
			}

			conductorsConfig[i] = true;

			auto start = std::chrono::steady_clock::now();
			Matrix2D<double>potentialField = setupInitialPotential(matrix, condCellsExtension, conductorsConfig);
			int iterations = computeFieldPotential(matrix, potentialField, initCells);
			auto end = std::chrono::steady_clock::now();
			double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			linearParam[i][i].CAirExtension = computeCapacityAndConductance(matrix, potentialField).x;

			std::cout << "\n\n";
			std::cout << "**********************************************************************************************************************************************************************\n";
			std::cout << "* > Potential field computation for air fill has been completed [conductor extension]" << "\n";
			std::cout << "* > Conductors config: " << conductorsConfig << "\n";
			std::cout << "* > The compute was done in " << iterations << " iterations\n";
			std::cout << "* > The compute was done in " << time / 1000 << " seconds\n";
			std::cout << "**********************************************************************************************************************************************************************\n";

			if (Tool::drawField == true)
			{
				drawField(potentialField);
			}
		}
	}

	// compute other elements in extension linear capacity matrix for air fill 
	for (int i = 0; i < condCells.getLength(); i++)
	{
		for (int j = 0; j < conductorsConfig.getLength(); j++)
		{
			conductorsConfig[j] = false;
		}
		conductorsConfig[i] = true;

		for (int j = i + 1; j < conductorsConfig.getLength(); j++)
		{
			bool computeTisConfig = true;

			for (int k = 0; k < symmetryConductors.getLength(); k++)
			{
				if (symmetryConductors[k].y == i)
				{
					linearParam[i][j].CAirExtension = linearParam[symmetryConductors[k].x][j].CAirExtension;
					linearParam[j][i].CAirExtension = linearParam[i][j].CAirExtension;
					computeTisConfig = false;
					break;
				}
			}

			if (computeTisConfig == true)
			{
				conductorsConfig[j] = true;

				Matrix2D<double>potentialField = setupInitialPotential(matrix, condCellsExtension, conductorsConfig);

				auto start = std::chrono::steady_clock::now();
				int iterations = computeFieldPotential(matrix, potentialField, initCells);
				auto end = std::chrono::steady_clock::now();
				double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

				linearParam[i][j].CAirExtension = (computeCapacityAndConductance(matrix, potentialField).x - linearParam[i][i].CAirExtension - linearParam[j][j].CAirExtension) / 2.0;
				linearParam[j][i].CAirExtension = linearParam[i][j].CAirExtension;

				std::cout << "\n\n";
				std::cout << "**********************************************************************************************************************************************************************\n";
				std::cout << "* > Potential field computation for air fill has been completed [conductor extension]" << "\n";
				std::cout << "* > Conductors config: " << conductorsConfig << "\n";
				std::cout << "* > The compute was done in " << iterations << " iterations\n";
				std::cout << "* > The compute was done in " << time / 1000 << " seconds\n";
				std::cout << "**********************************************************************************************************************************************************************\n";

				if (Tool::drawField == true)
				{
					drawField(potentialField);
				}

				conductorsConfig[j] = false;
			}
		}
	}

	// compute extension linear inductance
	Matrix2D<double> CAirExtensionMatrix(linearParam.getRows(), linearParam.getCols());
	for (int i = 0; i < CAirExtensionMatrix.getRows(); i++)
	{
		for (int j = 0; j < CAirExtensionMatrix.getCols(); j++)
		{
			CAirExtensionMatrix[i][j] = linearParam[i][j].CAirExtension;
		}
	}

	if (CAirExtensionMatrix.getRows() == 1)
	{
		linearParam[0][0].LExtension = Types::mu0 * Types::e0 * 1 / CAirExtensionMatrix[0][0];
	}
	else
	{
		Matrix2D<double> reverseCAirMatrix = MatrixTool::getReverseMatrix(CAirExtensionMatrix);
		for (int i = 0; i < linearParam.getRows(); i++)
		{
			for (int j = 0; j < linearParam.getCols(); j++)
			{
				linearParam[i][j].LExtension = Types::mu0 * Types::e0 * reverseCAirMatrix[i][j];
			}
		}
	}

	// compute conductor loss matrix
	Matrix2D<double> Rs(linearParam.getRows(), linearParam.getCols());
	for (int i = 0; i < Rs.getRows(); i++)
	{
		double mu = matrix[initCells[i].y][initCells[i].x].mu;
		double ro = matrix[initCells[i].y][initCells[i].x].ro;
		double dx = matrix[initCells[i].y][initCells[i].x].dx * 10.0e-3;
		double dL = linearParam[i][i].L - linearParam[i][i].LExtension;
		Rs[i][i] = sqrt(Types::pi * 1.0e9 * mu * ro);
		linearParam[i][i].R = (Rs[i][i] / Types::mu0) * (dL / dx);
		linearParam[i][i].R = abs(linearParam[i][i].R) * 10.0e-6;

		for (int j = i + 1; j < Rs.getCols(); j++)
		{
			mu = (matrix[initCells[i].y][initCells[i].x].mu + matrix[initCells[j].y][initCells[j].x].mu) / 2.0;
			ro = (matrix[initCells[i].y][initCells[i].x].ro + matrix[initCells[j].y][initCells[j].x].ro) / 2.0;
			dL = linearParam[i][j].L - linearParam[i][j].LExtension;
			Rs[i][j] = sqrt(Types::pi * 1.0e9 * mu * ro);
			linearParam[i][j].R = (Rs[i][j] / Types::mu0) * (dL / dx);
			linearParam[i][j].R = abs(linearParam[i][j].R) * 10.0e-6;
			linearParam[j][i].R = linearParam[i][j].R;
		}
	}
}



// Draw potential field
void GridSolver::drawField(const Matrix2D<double>& potentialField) const
{
	Vector <char> gradSymbols;
	gradSymbols.add((char)39);
	gradSymbols.add((char)96);
	gradSymbols.add((char)44);
	gradSymbols.add((char)46);
	gradSymbols.add((char)45);
	gradSymbols.add((char)43);
	gradSymbols.add((char)42);
	gradSymbols.add((char)101);
	gradSymbols.add((char)99);
	gradSymbols.add((char)97);
	gradSymbols.add((char)111);
	gradSymbols.add((char)56);
	gradSymbols.add((char)69);
	gradSymbols.add((char)83);
	gradSymbols.add((char)71);
	gradSymbols.add((char)81);
	gradSymbols.add((char)82);
	gradSymbols.add((char)66);
	gradSymbols.add((char)38);
	gradSymbols.add((char)37);
	gradSymbols.add((char)77);
	gradSymbols.add((char)87);
	gradSymbols.add((char)35);
	gradSymbols.add((char)64);

	Vector<double> gradValue(gradSymbols.getLength());
	for (int i = 0; i < gradValue.getLength() - 1; i++)
	{
		gradValue[i] = (Types::initHightPotential - Types::initLowPotential) / (gradValue.getLength() - 2) * i + Types::initLowPotential;
	}

	int cols = 0;

	std::cout << std::fixed << std::setprecision(3);
	for (int i = 0; i < gradValue.getLength() - 1; i++)
	{
		std::cout << gradSymbols[i] << " -> " << (double)gradValue[i] << "\t|\t";

		cols++;
		if (cols >= 4)
		{
			std::cout << "\n";
			cols = 0;
		}
	}
	std::cout << std::defaultfloat << std::setprecision(3);

	std::cout << "\n\t";
	for (int x = 0; x < potentialField.getCols(); x++)
	{
		if (x * 10 > potentialField.getCols())
		{
			break;
		}

		std::cout << ">" << x * 10;

		int number = (x * 10);
		int numberLength = 1;

		while (number /= 10)
		{
			numberLength++;
		}

		for (int i = 0; i < 9; i++)
		{
			if (i < 9 - numberLength)
			{
				std::cout << " ";
			}
			else
			{
				break;
			}
		}
	}

	std::cout << "\n\t";
	int xi = 0;
	for (int x = 0; x < potentialField.getCols(); x++)
	{
		if (xi > 0)
		{
			std::cout << xi;
		}
		else
		{
			std::cout << " ";
		}

		xi++;
		if (xi >= 10)
		{
			xi = 0;
		}
	}

	std::cout << "\n\n";
	for (int y = potentialField.getRows() - 1; y >= 0; y--)
	{
		std::cout << y << "\t";

		for (int x = 0; x < potentialField.getCols(); x++)
		{
			double value = potentialField[y][x];

			if (value < gradValue[0])
			{
				char symbol = gradSymbols[gradValue.getLength() - 1];
				std::cout << symbol;
			}
			else if (value == 1.0)
			{
				char symbol = char(219);
				std::cout << symbol;
			}
			else
			{
				for (int i = 0; i < gradValue.getLength() - 1; i++)
				{
					double lvalue = gradValue[i];
					double rvalue = gradValue[i + 1];
					if (value >= lvalue && value < rvalue)
					{
						char symbol = gradSymbols[i];
						std::cout << gradSymbols[i];
					}
				}
			}
		}
		if (y > 0)
		{
			std::cout << "\n";
		}
	}
}



// Print result info
void GridSolver::printResultInfo(
	const Point2D<int>& verticalSymmetryPoint,
	const Point2D<int>& horizontalSymmetryPoint,
	const Vector<Point2D<int>>& verticalSymmetryConductors,
	const Vector<Point2D<int>>& horizontalSymmetryConductors,
	const Vector<Point2D<int>>& symmetryConductors,
	const Matrix2D<Types::LinearParameters>& linearParam,
	Matrix2D<Types::PrimaryParameters> primaryParameters) const
{
	if (verticalSymmetryPoint != Point2D(0, 0) && horizontalSymmetryPoint != Point2D(0, 0) &&
		verticalSymmetryConductors.getLength() > 0 && horizontalSymmetryConductors.getLength() > 0)
	{
		std::cout << "The strip structure was defined as vertically and horizontally symmetrical\n";
		std::cout << "Vetrical symmetry point [leftX, rightX]: " << verticalSymmetryPoint << "\n";
		std::cout << "Horizontal symmetry point [bottomY, topY]: " << horizontalSymmetryConductors << "\n";
		std::cout << "Vertically symmetrical conductors: " << verticalSymmetryConductors;
		std::cout << "Horizontally symmetrical conductors: " << horizontalSymmetryConductors;
	}
	else if (verticalSymmetryPoint != Point2D(0, 0) && verticalSymmetryConductors.getLength() > 0)
	{
		std::cout << "The strip structure was defined as vertically symmetrical\n";
		std::cout << "Vetrical symmetry point [leftX, rightX]: " << verticalSymmetryPoint << "\n";
		std::cout << "Symmetrical conductors: " << symmetryConductors;
	}
	else if (horizontalSymmetryPoint != Point2D(0, 0) && horizontalSymmetryConductors.getLength() > 0)
	{
		std::cout << "The strip structure was defined as horizontal symmetrical\n";
		std::cout << "Horizontal symmetry point [bottomY, topY]: " << horizontalSymmetryConductors << "\n";
		std::cout << "Symmetrical conductors: " << symmetryConductors;
	}
	else
	{
		std::cout << "The strip structure was defined as non-symmetrical";
	}


	std::string gapStr = "     ";

	std::cout << "\n\n\n";
	std::cout << "- Linear parameters --------------------------------------------------------------------------------------------------------------------------------------------------";

	std::cout << "\n\n";
	std::cout << "C -> Linear capacity for dielectric fill [F/m]: " << "\n";
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		for (int j = 0; j < linearParam.getCols(); j++)
		{
			std::cout << Tool::doubleToString(linearParam[i][j].C) << gapStr;
		}
		if (i < linearParam.getRows() - 1)
		{
			std::cout << "\n\n";
		}
	}

	std::cout << "\n\n";
	std::cout << "C0 -> Linear capacity for air fill [F/m]: " << "\n";
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		for (int j = 0; j < linearParam.getCols(); j++)
		{
			std::cout << Tool::doubleToString(linearParam[i][j].CAir) << gapStr;
		}
		if (i < linearParam.getRows() - 1)
		{
			std::cout << "\n\n";
		}
	}

	std::cout << "\n\n";
	std::cout << "L -> Linear inductance [H/m]: " << "\n";
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		for (int j = 0; j < linearParam.getCols(); j++)
		{
			std::cout << Tool::doubleToString(linearParam[i][j].L) << gapStr;
		}
		if (i < linearParam.getRows() - 1)
		{
			std::cout << "\n\n";
		}
	}


	std::cout << "\n\n";
	std::cout << "G -> Dielectric losses [Sm/m]: " << "\n";
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		for (int j = 0; j < linearParam.getCols(); j++)
		{
			std::cout << Tool::doubleToString(linearParam[i][j].G) << gapStr;
		}
		if (i < linearParam.getRows() - 1)
		{
			std::cout << "\n\n";
		}
	}

	std::cout << "\n\n";
	std::cout << "R -> Conductor losses for 1 GHz [Ohms/m]: " << "\n";
	for (int i = 0; i < linearParam.getRows(); i++)
	{
		for (int j = 0; j < linearParam.getCols(); j++)
		{
			std::cout << Tool::doubleToString(linearParam[i][j].R) << gapStr;
		}
		if (i < linearParam.getRows() - 1)
		{
			std::cout << "\n\n";
		}
	}

	if (Tool::lossless == false)
	{
		std::cout << "\n\n\n";
		std::cout << "- Linear parameters for stip structure with extension conductors -----------------------------------------------------------------------------------------------------";

		std::cout << "\n\n";
		std::cout << "C0_Extension -> Linear capacity for air fill [F/m]: " << "\n";
		for (int i = 0; i < linearParam.getRows(); i++)
		{
			for (int j = 0; j < linearParam.getCols(); j++)
			{
				std::cout << Tool::doubleToString(linearParam[i][j].CAirExtension) << gapStr;
			}
			if (i < linearParam.getRows() - 1)
			{
				std::cout << "\n\n";
			}
		}

		std::cout << "\n\n";
		std::cout << "L_Extension -> Linear inductance [H/m]: " << "\n";
		for (int i = 0; i < linearParam.getRows(); i++)
		{
			for (int j = 0; j < linearParam.getCols(); j++)
			{
				std::cout << Tool::doubleToString(linearParam[i][j].LExtension) << gapStr;
			}
			if (i < linearParam.getRows() - 1)
			{
				std::cout << "\n\n";
			}
		}
	}

	std::cout << "\n\n\n";
	std::cout << "- Primary parameters -------------------------------------------------------------------------------------------------------------------------------------------------";


	if (linearParam.getRows() > 1)
	{
		std::cout << "\n\n";
		std::cout << "C1 -> Partial capacities for dielectric fill [F/m]: " << "\n";
		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			for (int j = 0; j < primaryParameters.getCols(); j++)
			{
				std::cout << Tool::doubleToString(primaryParameters[i][j].C1) << gapStr;
			}
			if (i < primaryParameters.getRows() - 1)
			{
				std::cout << "\n\n";
			}
		}

		std::cout << "\n\n";
		std::cout << "C1Air -> Partial capacities for air fill [F/m]: " << "\n";
		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			for (int j = 0; j < primaryParameters.getCols(); j++)
			{
				std::cout << Tool::doubleToString(primaryParameters[i][j].C1Air) << gapStr;
			}
			if (i < primaryParameters.getRows() - 1)
			{
				std::cout << "\n\n";
			}
		}
	}

	std::cout << "\n\n";
	std::cout << "epsEff -> Effective permittivity: " << "\n";
	long double epsEff = primaryParameters[0][0].epsEff;
	for (int i = 0; i < primaryParameters.getRows(); i++)
	{
		for (int j = 0; j < primaryParameters.getCols(); j++)
		{
			std::cout << Tool::doubleToString(primaryParameters[i][j].epsEff) << gapStr;
		}
		if (i < primaryParameters.getRows() - 1)
		{
			std::cout << "\n\n";
		}
	}

	if (linearParam.getRows() > 1)
	{
		std::cout << "\n\n";
		std::cout << "Z1 -> Wave impedance [Ohms]: " << "\n";
		for (int i = 0; i < primaryParameters.getRows(); i++)
		{
			for (int j = 0; j < primaryParameters.getCols(); j++)
			{
				std::cout << Tool::doubleToString(primaryParameters[i][j].Z1) << gapStr;
			}
			if (i < primaryParameters.getRows() - 1)
			{
				std::cout << "\n\n";
			}
		}
	}

	std::cout << "\n\n";
	std::cout << "Z0 -> Characteristic impedance [Ohms]: " << "\n";
	std::cout << Tool::doubleToString(primaryParameters[0][0].Z0);
}