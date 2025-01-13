#pragma once
#ifndef POINT2D_H
#define POINT2D_H

#include <iostream>

// Class 2D Point
template<typename T>
class Point2D
{
public:
	// Coordinate of 2D point
	T x, y;
	
	// Base constructor
	Point2D() {}

	// Constructor
	// x - coordinate x
	// y - coordinate y
	Point2D(T x, T y) : x(x), y(y) {}
	
	Point2D<T> operator = (Point2D<T> point)
	{
		return Point2D<T>(point.x, point.y);
	}

	Point2D<T> operator + (Point2D<T> point)
	{
		return Point2D<T>(this->x + point.x, this->y + point.y);
	}

	Point2D<T> operator - (Point2D<T> point)
	{
		return Point2D<T>(this->x - point.x, this->y - point.y);
	}

	Point2D<T> operator * (Point2D<T> point)
	{
		return Point2D<T>(this->x * point.x, this->y * point.y);
	}

	Point2D<T> operator / (Point2D<T> point)
	{
		return Point2D<T>(this->x / point.x, this->y / point.y);
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Point2D<T>& point)
{
	os << "[" << point.x << " ; " << point.y << "]";
	return os;
}

#endif // POINT2D_H
