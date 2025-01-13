#pragma once

// Class 2D matrix
template <typename T>
class Vector
{
public:

	// Base constructor
	Vector() { }

	// Constructor
	// rows - rows number
	// cols - columns number
	Vector(int length)
	{
		resize(length);
	}

	// Destructor
	~Vector()
	{
		deleteVector();
	}

	// Method delete and create new matrix with the specifide number of rows and columns
	// rows - rows number
	// cols - columns number
	void resize(int length)
	{
		deleteVector();

		_length = length;

		_vector = new T[length];
	}

	// Get rows number
	T getLength() const { return _length; }

	T& operator[](int index) const
	{
		if (index > _length - 1)
		{
			throw "Vector :: The out-of-bounds index";
		}
		return _vector[index];
	}

private:

	// Method delete 2D matrix
	void deleteVector()
	{
		if (_length > 0)
		{
			delete[] _vector;
		}
		_length = 0;
	}

	// Rows number
	int _length = 0;
	// Matrix
	T* _vector = nullptr;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vector)
{
	os << "[";
	for (int i = 0; i < vector.getLength(); i++)
	{
		os << vector[i];
		if (i < vector.getLength() - 1)
		{
			os << " ; ";
		}
	}
	os << "]";
	return os;
}