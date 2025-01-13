#pragma once
#ifndef MATRIX2D_H
#define MATRIX2D_H

#include <iostream>

// Class 2D matrix
template <typename T>
class Matrix2D
{
public:

	// Base constructor
	Matrix2D() { }

	// Constructor
	// rows - rows number
	// cols - columns number
	Matrix2D(int rows, int cols)
	{
		resize(rows, cols);
	}

	// Destructor
	~Matrix2D()
	{
		deleteMatrix();
	}

	// Method delete and create new matrix with the specifide number of rows and columns
	// rows - rows number
	// cols - columns number
	void resize(int rows, int cols)
	{
		deleteMatrix();

		_rows = rows;
		_cols = cols;

		_matrix = new T[rows * cols];
	}

	// Get rows number
	T getRows() const { return _rows; }

	// Get columns number
	T getCols() const { return _cols; }

	T* operator[](int rowIndex) const
	{
		if (rowIndex > _rows - 1)
		{
			throw "Vector :: The out-of-bounds matrix";
		}
		return _matrix + rowIndex * _cols;
	}

private:

	// Method delete 2D matrix
	void deleteMatrix()
	{
		if (_rows > 0)
		{
			delete[] _matrix;
		}
		_rows = 0;
		_cols = 0;
	}

	// Rows number
	int _rows = 0;
	// Columns number
	int _cols = 0;
	// Matrix
	T* _matrix = nullptr;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix2D<T>& matrix)
{
	for (int i = 0; i < matrix.getRows(); i++)
	{
		for (int j = 0; j < matrix.getCols(); j++)
		{
			os << matrix[i][j] << " ";
		}
		if (i < matrix.getRows() - 1)
		{
			os << "\n";
		}
	}
	return os;
}

#endif // !MATRIX2D_H
