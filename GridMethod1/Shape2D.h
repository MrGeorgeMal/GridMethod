#pragma once

#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <iostream>
#include "Vector.h"
#include "Point2D.h"
#include "Size2D.h"

// Parent class for 2D shapes
class Shape2D
{

#pragma region Constructors

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

#pragma endregion


#pragma region Getters Setters

public:

	void setOrigin(Point2D<double> origin) { _origin = origin; }

	void setOrigin(double originX, double originY) { _origin.x = originX; _origin.y = originY; }

	Point2D<double> getOrigin() { return _origin; }

#pragma endregion


#pragma region Protected Members

protected:

	// Shape origin
	Point2D<double> _origin;

#pragma endregion

};



// Line2D class - child from Shape2D
class Line2D : public Shape2D
{

#pragma region Constructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	Line2D() : Shape2D(),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// p1 - first line point
	// p2 - second line point
	Line2D(
		Point2D<double> p1,
		Point2D<double> p2) : Shape2D(),
		_p1(p1),
		_p2(p2) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// x1 - first X coordinate of point
	// y1 - first Y coordinate of point
	// x2 - second X coordinate of point
	// y2 - second Y coordinate of point
	Line2D(
		double x1,
		double y1,
		double x2,
		double y2) : Shape2D(),
		_p1(x1, y1),
		_p2(x2, y2) {}

	// Constructor. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	// origin - origin point
	Line2D(
		Point2D<double> origin) : Shape2D(origin),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0) {}

	// Constructor
	// origin - origin point
	// p1 - first line point
	// p2 - second line point
	Line2D(
		Point2D<double> origin,
		Point2D<double> p1,
		Point2D<double> p2) : Shape2D(origin),
		_p1(p1),
		_p2(p2) {}

	// Constructor
	// origin - origin point
	// x1 - first X coordinate of point
	// y1 - first Y coordinate of point
	// x2 - second X coordinate of point
	// y2 - second Y coordinate of point
	Line2D(
		Point2D<double> origin,
		double x1,
		double y1,
		double x2,
		double y2) : Shape2D(origin),
		_p1(x1, y1),
		_p2(x2, y2) {}

	// Constructor. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Line2D(
		double originX,
		double originY) : Shape2D(originX, originY),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0) {}

	// Constructor
	// originX - origin coordinate X
	// originY - origin coordinate Y
	// p1 - first line point
	// p2 - second line point
	Line2D(
		double originX,
		double originY,
		Point2D<double> p1,
		Point2D<double> p2) : Shape2D(originX, originY),
		_p1(p1),
		_p2(p2) {}

	// Constructor
	// originX - origin coordinate X
	// originY - origin coordinate Y
	// x1 - first X coordinate of point
	// y1 - first Y coordinate of point
	// x2 - second X coordinate of point
	// y2 - second Y coordinate of point
	Line2D(
		double originX,
		double originY,
		double x1,
		double y1,
		double x2,
		double y2) : Shape2D(originX, originY),
		_p1(x1, y1),
		_p2(x2, y2) {}

#pragma endregion


#pragma region Private Members

private:

	// Line2D points
	Point2D<double> _p1, _p2;

#pragma endregion

};

class Polygon : public Shape2D
{

#pragma region Contructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]. Empty points list
	Polygon() : Shape2D(), _points(0) {}

	// Base constructor. Empty points list
	// origin - origin point
	Polygon(Point2D<double> origin) : Shape2D(origin), _points(0) {}

	// Base constructor. Empty points list
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Polygon(
		double originX, 
		double originY) : Shape2D(originX, originY),
		_points(0) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// p1, p2, ... pn - next poligon points
	template <typename... Points>
	Polygon(
		Point2D<double> p1,
		Point2D<double> p2,
		Point2D<double> p3,
		Points... points) : Shape2D(), _points(3)
	{
		_points[0] = p1;
		_points[1] = p2;
		_points[2] = p3;
		(_points.add(points), ...);
	}

	// Constructor.
	// origin - origin point
	// p1, p2, ... pn - next poligon points
	template <typename... Points>
	Polygon(
		Point2D<double> origin,
		Point2D<double> p1,
		Point2D<double> p2,
		Point2D<double> p3,
		Points... points) : Shape2D(origin), _points(3)
	{
		_points[0] = p1;
		_points[1] = p2;
		_points[2] = p3;
		(_points.add(points), ...);
	}

	// Constructor.
	// originX - origin coordinate X
	// originY - origin coordinate Y
	// p1, p2, ... pn - next poligon points
	template <typename... Points>
	Polygon(
		double originX,
		double originY,
		Point2D<double> p1,
		Point2D<double> p2,
		Point2D<double> p3,
		Points... points) : Shape2D(originX, originY), _points(3)
	{
		_points[0] = p1;
		_points[1] = p2;
		_points[2] = p3;
		(_points.add(points), ...);
	}

#pragma endregion


#pragma region Public Methods

	// Add new points to polygon
	template <typename... Points>
	void addPoints(Points... points)
	{
		(_points.add(points), ...);
	}

#pragma endregion


#pragma region Getter Setter

public:
	Vector<Point2D<double>>& getPoints() { return _points; }

#pragma endregion


#pragma region Private Members

private:

	// Polygon points
	Vector<Point2D<double>> _points;

#pragma endregion

};



// Rectangle2D class - child from Shape2D
class Rectangle2D : public Shape2D
{

#pragma region Contructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]. Set rectangle points p1[0.0 ; 0.0], p2[0.0 ; 0.0], p3[0.0 ; 0.0], p4[0.0 ; 0.0]
	Rectangle2D() : Shape2D(),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0),
		_p3(0.0, 0.0),
		_p4(0.0, 0.0) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// p1 - first rectangle point
	// p2 - second rectangle point
	// p3 - third rectangle point
	// p4 - fourth rectangle point
	Rectangle2D(
		Point2D<double> p1,
		Point2D<double> p2,
		Point2D<double> p3,
		Point2D<double> p4) : Shape2D(),
		_p1(p1),
		_p2(p2),
		_p3(p3),
		_p4(p4) {}

	// Constructor. Set rectangle points p1[0.0 ; 0.0], p2[0.0 ; 0.0], p3[0.0 ; 0.0], p4[0.0 ; 0.0]
	// origin - origin point
	Rectangle2D(
		Point2D<double> origin) : Shape2D(origin),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0),
		_p3(0.0, 0.0),
		_p4(0.0, 0.0) {}

	// Constructor.
	// origin - origin point
	// p1 - first rectangle point
	// p2 - second rectangle point
	// p3 - third rectangle point
	// p4 - fourth rectangle point
	Rectangle2D(
		Point2D<double> origin,
		Point2D<double> p1,
		Point2D<double> p2,
		Point2D<double> p3,
		Point2D<double> p4) : Shape2D(origin),
		_p1(p1),
		_p2(p2),
		_p3(p3),
		_p4(p4) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// x1, y1 - first rectangle point
	// x2, y2 - second rectangle point
	// x3, y3 - third rectangle point
	// x4, y4 - fourth rectangle point
	Rectangle2D(
		double x1,
		double y1,
		double x2,
		double y2,
		double x3,
		double y3,
		double x4,
		double y4) : Shape2D(),
		_p1(x1, y1),
		_p2(x1, y1),
		_p3(x1, y1),
		_p4(x1, y1) {}

	// Constructor. Set rectangle points p1[0.0 ; 0.0], p2[0.0 ; 0.0], p3[0.0 ; 0.0], p4[0.0 ; 0.0]
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Rectangle2D(
		double originX,
		double originY) : Shape2D(originX, originY),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0),
		_p3(0.0, 0.0),
		_p4(0.0, 0.0) {
	}

	// Constructor.
	// origin - origin point
	// p1 - first rectangle point
	// p2 - second rectangle point
	// p3 - third rectangle point
	// p4 - fourth rectangle point
	Rectangle2D(
		double originX,
		double originY,
		double x1,
		double y1,
		double x2,
		double y2,
		double x3,
		double y3,
		double x4,
		double y4) : Shape2D(originX, originY),
		_p1(x1, y1),
		_p2(x1, y1),
		_p3(x1, y1),
		_p4(x1, y1) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// point - start rectangle point
	// size - rectangle size
	Rectangle2D(
		Point2D<double> point,
		Size2D<double> size) : Shape2D(),
		_p1(point),
		_size(size) {}

	// Constructor.
	// origin - origin point
	// point - start rectangle point
	// size - rectangle size
	Rectangle2D(
		Point2D<double> origin,
		Point2D<double> point,
		Size2D<double> size) : Shape2D(origin),
		_p1(point),
		_size(size) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// pointX, pointY - start rectangle point
	// width - rectangle width
	// height - rectangle height
	Rectangle2D(
		double pointX,
		double pointY,
		double width,
		double height) : Shape2D(),
		_p1(pointX, pointY),
		_size(width, height) {}

	// Constructor.
	// originX - origin coordinate X
	// originY - origin coordinate Y
	// pointX, pointY - start rectangle point
	// width - rectangle width
	// height - rectangle height
	Rectangle2D(
		double originX,
		double originY,
		double pointX,
		double pointY,
		double width,
		double height) : Shape2D(originX, originY),
		_p1(pointX, pointY),
		_size(width, height) {}

#pragma endregion


#pragma region Private Members

private:

	// Rectangle2D points
	Point2D<double> _p1, _p2, _p3, _p4;

	// Rectangle2D size
	Size2D<double> _size;

#pragma endregion

};

#endif // !SHAPE2D_H
