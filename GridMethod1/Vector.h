#pragma once
#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

// Class vector
template <typename T>
class Vector
{
public:

	// Base constructor
	Vector() { }

	// Constructor
	// length - vector length
	Vector(int length)
	{
		resize(length);
	}

    // Destructor
    ~Vector()
    {
       deleteVector();
    }

	// Method delete and create new vectoe with the specifide length
	// length - vector length
	void resize(int length)
	{
		deleteVector();

		_length = length;

		_vector = new T[length];
	}

	// Add new element to vector
	void add(T element)
	{
		_length++;
		T* temp = new T[_length];

		for (int i = 0; i < _length - 1; i++)
		{
			temp[i] = _vector[i];
		}
		temp[_length - 1] = element;

		resize(_length);

		for (int i = 0; i < _length; i++)
		{
			_vector[i] = temp[i];
		}

		delete[] temp;
	}

	// Get vector length
	int getLength() const { return _length; }

	T& operator[](int index) const
	{
		if (index > _length - 1)
		{
			throw "Vector :: The out-of-bounds index";
		}
		return _vector[index];
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector<T>& vector)
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

private:

	// Method delete vector
	void deleteVector()
	{
		if (_vector != nullptr)
		{
			delete[] _vector;
			_length = 0;
			_vector = nullptr;
		}
	}

	// Vector length
	int _length = 0;
	// Vector
	T* _vector = nullptr;
};

#endif // !VECTOR_H
