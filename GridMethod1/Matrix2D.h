#pragma once

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

		matrix = new T[rows * cols];
	}

	// Get rows number
	T getRows() const { return _rows; }

	// Get columns number
	T getCols() const { return _cols; }

	T* operator[](int rowIndex) const
	{
		return matrix + rowIndex * _cols;
	}

private:

	// Method delete 2D matrix
	void deleteMatrix()
	{
		if (_rows > 0)
		{
			delete[] matrix;
		}
		_rows = 0;
		_cols = 0;
	}

	// Rows number
	int _rows = 0;
	// Columns number
	int _cols = 0;
	// Matrix
	T* matrix = nullptr;
};