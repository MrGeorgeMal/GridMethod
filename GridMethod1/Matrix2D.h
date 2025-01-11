#pragma once

// Class 2D matrix
template <typename T>
class Matrix2D
{
public:
	Matrix2D();

	~Matrix2D();

	void Resize();

private:
	// Matrix size
	int rows, cols;
};