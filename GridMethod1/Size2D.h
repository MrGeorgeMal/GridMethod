#pragma once
#ifndef SIZE2D_H
#define SIZE2D_H

#include <iostream>

// Class 2D Size
template<typename T>
class Size2D
{
public:
	// Coordinate of 2D point
	T width, height;

	// Base constructor
	Size2D() {}

	// Constructor
	// width - width size
	// height - height size
	Size2D(T width, T height) : width(width), height(height) {}

	Size2D<T>& operator = (const Size2D<T>& size) {
		if (this != &size) {
			width = size.width;
			height = size.height;
		}
		return *this;
	}

	Size2D<T> operator + (Size2D<T> size)
	{
		return Size2D<T>(this->width + size.width, this->height + size.height);
	}

	Size2D<T> operator - (Size2D<T> size)
	{
		return Size2D<T>(this->width - size.width, this->height - size.height);
	}

	Size2D<T> operator * (Size2D<T> size)
	{
		return Size2D<T>(this->width * size.width, this->height * size.height);
	}

	Size2D<T> operator / (Size2D<T> size)
	{
		return Size2D<T>(this->width / size.width, this->height / size.height);
	}

	friend std::ostream& operator<<(std::ostream& os, const Size2D<T>& size)
	{
		os << "[" << size.width << " ; " << size.height << "]";
		return os;
	}
};

#endif // !SIZE2D_H
