#pragma once
#ifndef RECT2D_H
#define RECT2D_H

#include <iostream>

// Class 2D Size
template<typename T>
class Rect2D
{
public:
	// Coordinate of 2D point
	T left, top, right, bottom;

	// Base constructor
	Rect2D() {}

	// Constructor
	// left - left coordinate of rect
	// top - top coordinate of rect
	// right - right coordinate of rect
	// bottom - bottom coordinate of rect
	Rect2D(
		T left,
		T top,
		T right,
		T bottom) :
		left(left),
		top(top),
		right(right),
		bottom(bottom) {}

	Rect2D<T>& operator = (const Rect2D<T>& rect) {
		if (this != &rect) {
			left = rect.left;
			top = rect.top;
			right = rect.right;
			bottom = rect.bottom;
		}
		return *this;
	}

	Rect2D<T> operator + (Rect2D<T> rect)
	{
		return Size2D<T>(
			this->left + rect.left,
			this->top + rect.top,
			this->right + rect.right,
			this->bottom + rect.bottom);
	}

	Rect2D<T> operator - (Rect2D<T> rect)
	{
		return Size2D<T>(
			this->left - rect.left,
			this->top - rect.top,
			this->right - rect.right,
			this->bottom - rect.bottom);
	}

	Rect2D<T> operator * (Rect2D<T> rect)
	{
		return Size2D<T>(
			this->left * rect.left,
			this->top * rect.top,
			this->right * rect.right,
			this->bottom * rect.bottom);
	}

	Rect2D<T> operator / (Rect2D<T> rect)
	{
		return Size2D<T>(
			this->left / rect.left,
			this->top / rect.top,
			this->right / rect.right,
			this->bottom / rect.bottom);
	}

	friend std::ostream& operator<<(std::ostream& os, const Rect2D<T>& rect)
	{
		os << "[" << rect.left << " ; " << rect.top << " ; " << rect.right << " ; " << rect.bottom << "]";
		return os;
	}
};

#endif // !RECT2D_H
