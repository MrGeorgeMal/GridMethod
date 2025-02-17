#pragma once

#ifndef SHAPE2D_H
#define SHAPE2D_H

#include "Vector.h"
#include "Point2D.h"
#include "Size2D.h"
#include "Material.h"

// Parent class for 2D shapes
class Shape2D
{

#pragma region Constructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]
	Shape2D() : _origin(0.0, 0.0), _material(new Dielectric()) {}

	// Constructor
	// origin - origin point
	Shape2D(Point2D<double> origin) : _origin(origin), _material(new Dielectric()) {}

	// Constructor
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Shape2D(double originX, double originY) : _origin(originX, originY), _material(new Dielectric()) {}

#pragma endregion

#pragma region Public Methods

public:

	// Get object type
	virtual const char* getType() const = 0;

#pragma endregion


#pragma region Getters Setters

public:

	void setOrigin(Point2D<double> origin) { _origin = origin; }

	void setOrigin(double originX, double originY) { _origin.x = originX; _origin.y = originY; }

	Point2D<double> getOrigin() { return _origin; }

	// Set material
	void setMaterial(Material* material) { _material = material; }

	// Get material
	Material* getMaterial() { return _material; }


#pragma endregion


#pragma region Protected Members

protected:

	// Shape origin
	Point2D<double> _origin;

	Material* _material;

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
	
	// Constructor. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	// origin - origin point
	Line2D(
		Point2D<double> origin) : Shape2D(origin),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0) {
	}

	// Constructor. Set line points p1[0.0 ; 0.0] and p2[0.0 ; 0.0]
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Line2D(
		double originX,
		double originY) : Shape2D(originX, originY),
		_p1(0.0, 0.0),
		_p2(0.0, 0.0) {
	}

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

#pragma region Public Methods

public:

	const char* getType() const override { return "Line2D"; }

#pragma endregion


#pragma region Getter Setter

public:

	// Get line first point
	Point2D<double> getP1() { return _p1; }

	// Get line first point
	Point2D<double> getP2() { return _p2; }

#pragma endregion


#pragma region Private Members

private:

	// Line2D points
	Point2D<double> _p1, _p2;

#pragma endregion

};

class Polygon2D : public Shape2D
{

#pragma region Contructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]. Empty points list
	Polygon2D() : Shape2D(), _points(0) {}

	// Base constructor. Empty points list
	// origin - origin point
	Polygon2D(Point2D<double> origin) : Shape2D(origin), _points(0) {}

	// Base constructor. Empty points list
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Polygon2D(
		double originX, 
		double originY) : Shape2D(originX, originY),
		_points(0) {}

	// Constructor. Set origin [0.0 ; 0.0]
	// p1, p2, ... pn - next poligon points
	template <typename... Points>
	Polygon2D(
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
	Polygon2D(
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

#pragma endregion


#pragma region Public Methods

	const char* getType() const override { return "Polygon2D"; }

	// Add new points to polygon
	template <typename... Points>
	void addPoints(Points... points)
	{
		(_points.add(points), ...);
	}

#pragma endregion


#pragma region Getter Setter

public:

	// Get vector of shape points
	Vector<Point2D<double>>& getPoints() { return _points; }

#pragma endregion


#pragma region Protected Members

protected:

	// Polygon points
	Vector<Point2D<double>> _points;

#pragma endregion

};



// Rectangle2D class - child from Polygon
class Rectangle2D : public Polygon2D
{

#pragma region Contructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]. Set four points of rectangle [0.0; 0.0], [0.0; 1.0], [1.0; 1.0], [1.0; 0.0].
	Rectangle2D() : Polygon2D()
	{
		_points.add(Point2D<double>(0.0, 0.0));
		_points.add(Point2D<double>(0.0, 1.0));
		_points.add(Point2D<double>(1.0, 1.0));
		_points.add(Point2D<double>(1.0, 0.0));
	}

	// Constructor. Set four points of rectangle [0.0; 0.0], [0.0; 1.0], [1.0; 1.0], [1.0; 0.0].
	// origin - origin point
	Rectangle2D(Point2D<double> origin) : Polygon2D(origin)
	{
		_points.add(Point2D<double>(0.0, 0.0));
		_points.add(Point2D<double>(0.0, 1.0));
		_points.add(Point2D<double>(1.0, 1.0));
		_points.add(Point2D<double>(1.0, 0.0));
	}

	// Constructor. Set four points of rectangle [0.0; 0.0], [0.0; 1.0], [1.0; 1.0], [1.0; 0.0].
	// originX - origin coordinate X
	// originY - origin coordinate Y
	Rectangle2D(
		double originX,
		double originY) : Polygon2D(originX, originY)
	{
		_points.add(Point2D<double>(0.0, 0.0));
		_points.add(Point2D<double>(0.0, 1.0));
		_points.add(Point2D<double>(1.0, 1.0));
		_points.add(Point2D<double>(1.0, 0.0));
	}

	// Constructor. Set origin [0.0 ; 0.0].
	// point - start rectangle point
	// size - rectangle size
	Rectangle2D(
		Point2D<double> point,
		Size2D<double> size) : Polygon2D()
	{
		Point2D<double> p1 = point;
		Point2D<double> p2 = Point2D<double>(p1.x, p1.y + size.height);
		Point2D<double> p3 = Point2D<double>(p1.x + size.width, p1.y + size.height);
		Point2D<double> p4 = Point2D<double>(p1.x + size.width, p1.y);

		_points.add(p1);
		_points.add(p2);
		_points.add(p3);
		_points.add(p4);
	}

	// Constructor. Set origin [0.0 ; 0.0].
	// pointX, pointY - start rectangle point
	// width - rectangle width
	// height - rectangle height
	Rectangle2D(
		double pointX,
		double pointY,
		double width,
		double height,
		Size2D<double> size) : Polygon2D()
	{
		Point2D<double> p1 = Point2D<double>(pointX, pointY);
		Point2D<double> p2 = Point2D<double>(pointX, pointY + height);
		Point2D<double> p3 = Point2D<double>(pointX + width, pointY + height);
		Point2D<double> p4 = Point2D<double>(pointX + width, pointY);

		_points.add(p1);
		_points.add(p2);
		_points.add(p3);
		_points.add(p4);
	}

	// Constructor.
	// origin - origin point
	// point - start rectangle point
	// size - rectangle size
	Rectangle2D(
		Point2D<double> origin,
		Point2D<double> point,
		Size2D<double> size) : Polygon2D(origin)
	{
		Point2D<double> p1 = point;
		Point2D<double> p2 = Point2D<double>(p1.x, p1.y + size.height);
		Point2D<double> p3 = Point2D<double>(p1.x + size.width, p1.y + size.height);
		Point2D<double> p4 = Point2D<double>(p1.x + size.width, p1.y);

		_points.add(p1);
		_points.add(p2);
		_points.add(p3);
		_points.add(p4);
	}

	// Constructor. Set origin [0.0 ; 0.0].
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
		double height,
		Size2D<double> size) : Polygon2D(originX, originY)
	{
		Point2D<double> p1 = Point2D<double>(pointX, pointY);
		Point2D<double> p2 = Point2D<double>(pointX, pointY + height);
		Point2D<double> p3 = Point2D<double>(pointX + width, pointY + height);
		Point2D<double> p4 = Point2D<double>(pointX + width, pointY);

		_points.add(p1);
		_points.add(p2);
		_points.add(p3);
		_points.add(p4);
	}

#pragma endregion


#pragma region Public Methods

	const char* getType() const override { return "Rectangle2D"; }

#pragma endregion


#pragma region Private Methods

private:

	// Make method private
	using Polygon2D::addPoints;

#pragma endregion


#pragma region Getter Setter

public:

	// Get rectangle start point
	Point2D<double> getPoint() { return _points[0]; }

	// Set start point
	void setPoint(Point2D<double> point) { _points[0] = point; }

	// Set start point
	void setPoint(double x, double y) { _points[0] = Point2D<double>(x, y); }

	// Get rectangle size
	Size2D<double> getSize() { return _size; }

	// Set rectangle size
	void setSize(Size2D<double> size) { _size = size; }

	// Set rectangle size
	void setSize(double width, double height) { _size = Size2D<double>(width, height); }

	// Get rectangle width
	double getWidth() { return _size.width; }

	// Set rectangle width
	void setWidth(double width) { _size.width = width; }

	// Get rectangle height
	double geHeight() { return _size.height; }

	// Set rectangle height
	void setHeight(double height) { _size.height = height; }

#pragma endregion


#pragma region Private Members

private:

	// Rectangle2D size
	Size2D<double> _size;

#pragma endregion

};

#endif // !SHAPE2D_H
