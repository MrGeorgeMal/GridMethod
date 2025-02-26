#pragma once

#ifndef SHAPE2D_H
#define SHAPE2D_H

#include <iostream>
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

#pragma endregion

#pragma region Public Methods

public:

	// Get object type
	virtual const char* getType() const = 0;

	// Move origin to point (move all shape point)
	virtual void moveOrigin(Point2D<double> point)
	{
		_origin = point;
	}

	// Get shape copy pointer
	virtual Shape2D* getCopy() const = 0;

#pragma endregion


#pragma region Getters Setters

public:

	void setOrigin(Point2D<double> origin) { _origin = origin; }

	void setOrigin(double originX, double originY) { _origin.x = originX; _origin.y = originY; }

	Point2D<double> getOrigin() { return _origin; }

	// Set material
	void setMaterial(Material* material) { _material = material; }

	// Get material
	Material* getMaterial() const { return _material; }


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
	
	// Constructor. Set origin [0.0 ; 0.0]
	// p1 - first line point
	// p2 - second line point
	Line2D(
		Point2D<double> p1,
		Point2D<double> p2) : Shape2D(),
		_p1(p1),
		_p2(p2) {}

#pragma endregion


#pragma region Public Methods

public:

	// Get object type
	const char* getType() const override { return "Line2D"; }

	// Move origin to point (move all shape point)
	void moveOrigin(Point2D<double> point) override
	{
		Point2D<double> offset = point - _origin;
		setP1(_p1 + offset);
		setP2(_p2 + offset);
		Shape2D::moveOrigin(point);
	}

	// Get shape copy pointer
	Shape2D* getCopy() const override
	{
		Line2D* line = new Line2D();
		line->_origin = _origin;
		line->_material = _material;
		line->_p1 = _p1;
		line->_p2 = _p2;
		return line;
	}

	friend std::ostream& operator<<(std::ostream& os, const Line2D& shape)
	{
		os << "| Line2D (" << &shape << "):\n";
		if (shape._material->getType() == "Dielectric")
		{
			Dielectric* material = dynamic_cast<Dielectric*>(shape._material);
			os << "|-- material -> " << *material << "\n";
		}
		if (shape._material->getType() == "Conductor")
		{
			Conductor* material = dynamic_cast<Conductor*>(shape._material);
			os << "|-- material -> " << *material << "\n";
		}
		os << "|-- point 1 -> " << shape._p1 << "\n";
		os << "|-- point 2 -> " << shape._p2 << "\n";
		os << "|-- origin -> " << shape._origin;
		return os;
	}

#pragma endregion


#pragma region Getter Setter

public:

	// Get line first point
	Point2D<double> getP1() const { return _p1; }

	// Get line first point
	Point2D<double> getP2() const { return _p2; }

	void setP1(Point2D<double> point) { _p1 = point; }

	void setP2(Point2D<double> point) { _p2 = point; }

#pragma endregion


#pragma region Private Members

private:

	// Line2D points
	Point2D<double> _p1, _p2;

#pragma endregion

};



// Polygon2D class - child from Shape2D
class Polygon2D : public Shape2D
{

#pragma region Contructors

public:

	// Base constructor. Set origin [0.0 ; 0.0]. Empty points list
	Polygon2D() : Shape2D(), _points(0) {}

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

#pragma endregion


#pragma region Public Methods

	// Get object type
	const char* getType() const override { return "Polygon2D"; }

	// Move origin to point (move all shape point)
	void moveOrigin(Point2D<double> point) override
	{
		Point2D<double> offset = point - _origin;
		for (int i = 0; i < _points.getLength(); i++)
		{
			_points[i] = _points[i] + offset;
		}
		Shape2D::moveOrigin(point);
	}

	// Get shape copy pointer
	Shape2D* getCopy() const override
	{
		Polygon2D* polygon = new Polygon2D();
		polygon->_origin = _origin;
		polygon->_material = _material;
		for (int i = 0; i < _points.getLength(); i++)
		{
			polygon->addPoints(_points[i]);
		}
		return polygon;
	}

	// Add new points to polygon
	template <typename... Points>
	void addPoints(Points... points)
	{
		(_points.add(points), ...);
	}

	friend std::ostream& operator<<(std::ostream& os, const Polygon2D& shape)
	{
		os << "| Polygon2D (" << &shape << "):\n";
		if (shape._material->getType() == "Dielectric")
		{
			Dielectric* material = dynamic_cast<Dielectric*>(shape._material);
			os << "|-- material -> " << *material << "\n";
		}
		if (shape._material->getType() == "Conductor")
		{
			Conductor* material = dynamic_cast<Conductor*>(shape._material);
			os << "|-- material -> " << *material << "\n";
		}
		for (int i = 0; i < shape._points.getLength(); i++)
		{
			os << "|-- point " << i + 1 << ": " << shape._points[i] << "\n";
		}
		os << "|-- origin -> " << shape._origin;
		return os;
	}

#pragma endregion


#pragma region Getter Setter

public:

	// Get vector of shape points
	const Vector<Point2D<double>>& getPoints() const { return _points; }

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

	// Constructor. Set origin [0.0 ; 0.0].
	// point - start rectangle point
	// size - rectangle size
	Rectangle2D(
		Point2D<double> point,
		Size2D<double> size) : Polygon2D(), _size(size)
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

#pragma endregion


#pragma region Public Methods

public:

	// Get object type
	const char* getType() const override { return "Rectangle2D"; }

	// Get shape copy pointer
	Shape2D* getCopy() const override
	{
		Rectangle2D* rectangle = new Rectangle2D();
		rectangle->_origin = _origin;
		rectangle->_material = _material;
		rectangle->_isScreen = _isScreen;
		rectangle->_points[0] = _points[0];
		rectangle->_points[1] = _points[1];
		rectangle->_points[2] = _points[2];
		rectangle->_points[3] = _points[3];
		rectangle->_size = _size;
		return rectangle;
	}

	friend std::ostream& operator<<(std::ostream& os, const Rectangle2D& shape)
	{
		os << "| Rectangle2D (" << &shape << "):\n";
		if (shape._material->getType() == "Dielectric")
		{
			Dielectric* material = dynamic_cast<Dielectric*>(shape._material);
			os << "|-- material -> " << *material << "\n";
		}
		if (shape._material->getType() == "Conductor")
		{
			Conductor* material = dynamic_cast<Conductor*>(shape._material);
			os << "|-- material -> " << *material << "\n";
		}
		os << "|-- isScreen -> " << shape._isScreen << "\n";
		os << "|-- point -> " << shape._points[0] << "\n";
		os << "|-- size -> " << shape._size << "\n";
		os << "|-- origin -> " << shape._origin;

		return os;
	}

#pragma endregion


#pragma region Private Methods

private:

	// Make method private
	using Polygon2D::addPoints;

	// Update points
	void updatePoints()
	{
		Point2D<double> p1 = _points[0];
		Point2D<double> p2 = Point2D<double>(p1.x, p1.y + _size.height);
		Point2D<double> p3 = Point2D<double>(p1.x + _size.width, p1.y + _size.height);
		Point2D<double> p4 = Point2D<double>(p1.x + _size.width, p1.y);

		_points[0] = p1;
		_points[1] = p2;
		_points[2] = p3;
		_points[3] = p4;
	}

#pragma endregion


#pragma region Getter Setter

public:

	// Get rectangle start point
	Point2D<double> getPoint() { return _points[0]; }

	// Set start point
	void setPoint(Point2D<double> point)
	{
		_points[0] = point;
		updatePoints();
	}

	// Get rectangle size
	Size2D<double> getSize() { return _size; }

	// Set rectangle size
	void setSize(Size2D<double> size)
	{
		_size = size;
		updatePoints();
	}

	// Get rectangle width
	double getWidth() { return _size.width; }

	// Set rectangle width
	void setWidth(double width)
	{
		_size.width = width;
		updatePoints();
	}

	// Get rectangle height
	double getHeight() { return _size.height; }

	// Set rectangle height
	void setHeight(double height)
	{
		_size.height = height;
		updatePoints();
	}

	// Check if the rectangle is a screen
	bool isScreen() { return _isScreen; }

	// Make or remove a mark to use a rectangle as a screen
	void makeAsScreen(bool isScreen) { _isScreen = isScreen; }

#pragma endregion


#pragma region Private Members

private:

	// Rectangle2D size
	Size2D<double> _size;

	// This rectangle is screen?
	bool _isScreen = false;

#pragma endregion

};

#endif // !SHAPE2D_H
