#include "Rasterizer.h"



// Get discretized point
Point2D<int> Rasterizer::discretizePoint(const Point2D<double> point) const
{
	double dx = _cell.width;
	double dy = _cell.height;

	int x = floor(point.x / dx);
	int y = floor(point.y / dy);

	return Point2D<int>(x, y);
}



// Get discretized size
Size2D<int> Rasterizer::discretizeSize(const Size2D<double> size) const
{
	double dx = _cell.width;
	double dy = _cell.height;

	int width = floor(size.width / dx);
	int height = floor(size.height / dy);

	return Size2D<int>(width, height);
}



// Define matrix size
Size2D<int> Rasterizer::defineMatrixSize(const Vector<Shape2D> shapes) const
{
	// Try to find a screen rectangle

	// If there is no screen

	double minX = shapes[0].getPoints()[0].x;
	double minY = polygon2d->getPoints()[0].y;
	double maxX = 0;
	double maxY = 0;

	for (int i = 0; i < shapes.getLength(); i++)
	{

	}
}



// Rasterize shape 2D
Matrix2D<Rasterizer::CellInfo> Rasterizer::rasterize(Vector<Shape2D> shapes) const
{
	// Try to find a screen rectangle (realize in StripStructure class)
	Rectangle2D* screen = nullptr;
	for (int i = 0; i < shapes.getLength(); i++)
	{
		if (shapes[i].getType() == "Rectangle2D")
		{
			screen = dynamic_cast<Rectangle2D*>(&shapes[i]);
		}
	}

	// Copy all shapes to a new vector and offset them to the [0 ; 0] coordinate

	// Create matrix

	// Rasterize all shapes



	for (int i = 0; i < shapes.getLength(); i++)
	{
		Line2D* line2d = dynamic_cast<Line2D*>(shapes[i]);
	}
	if (shape2d->getType() == "Line2D")
	{
		Line2D* line2d = dynamic_cast<Line2D*>(shape2d);
		return rasterizeLine(line2d);
	}

	else if (shape2d->getType() == "Polygon2D" || shape2d->getType() == "Rectangle2D")
	{
		Polygon2D* poligon2d = dynamic_cast<Polygon2D*>(shape2d);
		return rasterizePolygon(poligon2d);
	}

	else
	{
		return Matrix2D<Rasterizer::CellInfo>();
	}
}



// Rasterize line shape
Matrix2D<Material> Rasterizer::rasterizeLine(const Line2D* line2d) const
{
	Size2D<int> size = defineMatrixSize(line2d);
	
	if (_antialiasing)
	{
		return drawLineWu(line2d);
	}
	else
	{
		return drawLineBresenham(line2d);
	}
}



// Rasterize polygon shape
Matrix2D<Material> Rasterizer::rasterizePolygon(const Polygon2D* polygon2d) const
{
	if (polygon2d->getPoints().getLength() < 3)
	{
		return Matrix2D<Material>();
	}

	if (_antialiasing)
	{
		drawPolygonSSAA(polygon2d);
	}
	else
	{
		drawPolygon(polygon2d);
	}
}

// Rasterize line, Bresenham method
Matrix2D<Material> Rasterizer::drawLineBresenham(const Line2D* line2d) const
{
	return Matrix2D<Material>();
}


// Rasterize line, Wu method
Matrix2D<Material> Rasterizer::drawLineWu(const Line2D* line2d) const
{
	return Matrix2D<Material>();
}



// Rasterize polygon, no antialiasing method
Matrix2D<Material> Rasterizer::drawPolygon(const Polygon2D* polygon2d) const
{
	return Matrix2D<Material>();
}



// Rasterize polygon, super sampled antialiasing method
Matrix2D<Material> Rasterizer::drawPolygonSSAA(const Polygon2D* polygon2d) const
{
	return Matrix2D<Material>();
}


/*
// Define matrix size
Size2D<int> Rasterizer::defineLineSize(const Line2D* line2d) const
{
	double minX = line2d->getP1().x;
	double minY = line2d->getP1().y;
	double maxX = line2d->getP2().x;
	double maxY = line2d->getP2().y;

	if (line2d->getP2().x < minX)
	{
		minX = line2d->getP2().x;
		maxX = line2d->getP1().x;
	}
	if (line2d->getP2().y < minY)
	{
		minY = line2d->getP2().y;
		maxY = line2d->getP1().y;
	}

	int width = floor(maxX - minX);
	int height = floor(maxY - minY);

	return Size2D<int>(width, height);
}



// Define matrix size
Size2D<int> Rasterizer::definePolygonSize(const Polygon2D* polygon2d) const
{
	double minX = polygon2d->getPoints()[0].x;
	double minY = polygon2d->getPoints()[0].y;
	double maxX = 0;
	double maxY = 0;

	for (int i = 0; i < polygon2d->getPoints().getLength(); i++)
	{
		minX = std::min(minX, polygon2d->getPoints()[i].x);
		minY = std::min(minY, polygon2d->getPoints()[i].y);
		maxX = std::max(maxX, polygon2d->getPoints()[i].x);
		maxY = std::max(maxY, polygon2d->getPoints()[i].y);
	}

	int width = floor(maxX - minX);
	int height = floor(maxY - minY);

	return Size2D<int>(width, height);
}
*/