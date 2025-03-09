#pragma once
#ifndef MATRIXTOOL_H
#define MATRIXTOOL_H

#include "Matrix2D.h"

static class MatrixTool
{
public:
	// Get transpone matrix
	static Matrix2D<double> getTranspone(const Matrix2D<double>& matrix)
	{
		int rows = matrix.getRows();
		int cols = matrix.getCols();
		Matrix2D<double> tMatrix(rows, cols);

		for (int i = 0; i < cols; i++)
		{
			for (int j = 0; j < rows; j++)
			{
				tMatrix[i][j] = matrix[j][i];
			}
		}
		return tMatrix;
	}



	// Get minor matrix
	static Matrix2D<double> getMinorMatrix(const Matrix2D<double>& matrix, int row, int col)
	{
		int rows = matrix.getRows();
		int cols = matrix.getCols();
		Matrix2D<double> mMatrix(rows - 1, cols - 1);

		int di = 0, dj = 0;

		for (int i = 0; i < rows - 1; i++)
		{
			if (i == row)
			{
				di = 1;
			}
			dj = 0;

			for (int j = 0; j < cols - 1; j++)
			{
				if (j == col)
				{
					dj = 1;
				}

				mMatrix[i][j] = matrix[i + di][j + dj];
			}
		}

		return mMatrix;
	}



	// Get determinant
	static double getDeterminant(const Matrix2D<double>& matrix)
	{
		if (matrix.getRows() != matrix.getCols())
		{
			throw "MatrixTool::getDeterminant::Matrix is not quadratic";
			return 0.0;
		}

		int m = matrix.getRows();
		int k = 1;
		double determinant = 0.0;

		if (m < 1)
		{
			throw "MatrixTool::getDeterminant::Unable to calculate the determinant";
			return 0.0;
		}
		if (m == 1)
		{
			determinant = matrix[0][0];
			return determinant;
		}
		if (m == 2)
		{
			determinant = matrix[0][0] * matrix[1][1] - (matrix[1][0] * matrix[0][1]);
			return determinant;
		}
		if (m > 2)
		{
			for (int i = 0; i < m; i++)
			{
				Matrix2D<double> dMatrix(getMinorMatrix(matrix, i, 0));
				double tempDeterminant = getDeterminant(dMatrix);
				determinant = determinant + k * matrix[i][0] * tempDeterminant;
				k = -k;
			}
		}
		return determinant;
	}



	static Matrix2D<double> getReverseMatrix(const Matrix2D<double>& matrix)
	{
		int m = matrix.getRows();
		Matrix2D<double> rMatrix(m, m);

		if (matrix.getRows() != matrix.getCols())
		{
			throw "MatrixTool::getReverseMatrix::Matrix is not quadratic";
			return rMatrix;
		}

		double determinant = getDeterminant(matrix);

		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < m; j++)
			{
				rMatrix[i][j] = getDeterminant(getMinorMatrix(matrix, i, j));
				// if the sum of row and column indices is odd
				// change sign
				if ((i + j) % 2 == 1)
				{
					rMatrix[i][j] = -rMatrix[i][j];
				}
				rMatrix[i][j] = rMatrix[i][j] / determinant;
			}
		}

		return getTranspone(rMatrix);
	}
};

#endif // !MATRIXTOOL_H