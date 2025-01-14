#pragma once

#ifndef SHAPE2D_H
#define SHAPE2D_H

#include "Point2D.h"

// Parent class for 2D shapes
class Shape2D
{
public:
	// Base constructor. Set origin [0.0 ; 0.0]
	Shape2D() : _origin(0.0, 0.0) {}

	// Constructor
	// origin - origin point
	Shape2D(Point2D<double> origin) : _origin(origin) {}

	// Constructor
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Shape2D(double originX, double originY) : _origin(originX, originY) {}

	void setOrigin(Point2D<double> origin) { _origin = origin; }

	void setOrigin(double originX, double originY) { _origin.x = originX; _origin.y = originY; }

	Point2D<double> getOrigin() { return _origin; }

protected:
	// Shape origin
	Point2D<double> _origin;
};



// Child 2D line class
class Line2D : public Shape2D
{
public:
	// Base constructor. Set origin [0.0 ; 0.0]. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	Line2D() : Shape2D(), _p1(0.0, 0.0), _p2(0.0, 0.0) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// p1 - first line point
	// p2 - second line point
	Line2D(Point2D<double> p1, Point2D<double> p2) : Shape2D(), _p1(p1), _p2(p2) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// x1 - first X coordinate of point
	// y1 - first Y coordinate of point
	// x2 - second X coordinate of point
	// y2 - second Y coordinate of point
	Line2D(double x1, double y1, double x2, double y2) : Shape2D(), _p1(x1, y1), _p2(x2, y2) {}

	// Constructor. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	// origin - origin point
	Line2D(Point2D<double> origin) : Shape2D(origin), _p1(0.0, 0.0), _p2(0.0, 0.0) {}

	// Constructor
	// origin - origin point
	// p1 - first line point
	// p2 - second line point
	Line2D(Point2D<double> origin, Point2D<double> p1, Point2D<double> p2) : Shape2D(origin), _p1(p1), _p2(p2) {}

	// Constructor
	// origin - origin point
	// x1 - first X coordinate of point
	// y1 - first Y coordinate of point
	// x2 - second X coordinate of point
	// y2 - second Y coordinate of point
	Line2D(Point2D<double> origin, double x1, double y1, double x2, double y2) : Shape2D(origin), _p1(x1, y1), _p2(x2, y2) {}

	// Constructor. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Line2D(double originX, double originY) : Shape2D(originX, originY), _p1(0.0, 0.0), _p2(0.0, 0.0) {}

	// Constructor
	// originX - origin coordinate X
	// originY - origin coordinate Y
	// p1 - first line point
	// p2 - second line point
	Line2D(double originX, double originY, Point2D<double> p1, Point2D<double> p2) : Shape2D(originX, originY), _p1(p1), _p2(p2) {}

	// Constructor
	// originX - origin coordinate X
	// originY - origin coordinate Y
	// x1 - first X coordinate of point
	// y1 - first Y coordinate of point
	// x2 - second X coordinate of point
	// y2 - second Y coordinate of point
	Line2D(double originX, double originY, double x1, double y1, double x2, double y2) : Shape2D(originX, originY), _p1(x1, y1), _p2(x2, y2) {}

private:
	// Line2D points
	Point2D<double> _p1, _p2;
};



// Child 2D rectangle class
class Rectangle2D : public Shape2D
{
public:
	Rectangle2D() : Shape2D(), _p1(0.0, 0.0), _p2(0.0, 0.0), _p3(0.0, 0.0), _p4(0.0, 0.0) {}

private:
	// Rectangle2D points
	Point2D<double> _p1, _p2, _p3, _p4;
};

#endif // !SHAPE2D_H
