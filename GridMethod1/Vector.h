#pragma once
#ifndef VECTOR_H
#define VECTOR_H


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

	// Get vector length
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

	// Method delete vector
	void deleteVector()
	{
		if (_length > 0)
		{
			delete[] _vector;
		}
		_length = 0;
	}

	// Vector length
	int _length = 0;
	// Vector
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

#endif // !VECTOR_H
