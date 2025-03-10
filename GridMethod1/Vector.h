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
	Vector()
	{
	}

	// Constructor
	// length - vector length
	Vector(int length)
	{
		resize(length);
	}

	
	Vector(const Vector<T>& vector)
	{
		for (int i = 0; i < vector.getLength(); i++)
		{
			add(vector[i]);
		}
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

		resize(_length);

		for (int i = 0; i < _length - 1; i++)
		{
			_vector[i] = temp[i];
		}

		delete[] temp;

		_vector[_length - 1] = element;
	}

	// Add new element to vector
	void addToHead(T element)
	{
		_length++;
		T* temp = new T[_length];

		temp[0] = element;
		for (int i = 1; i < _length; i++)
		{
			temp[i] = _vector[i - 1];
		}

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

	Vector<T>& operator = (Vector<T>& vector)
	{
		for (int  i = 0; i < vector.getLength(); i++)
		{
			this->add(vector[i]);
		}
		return *this;
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
