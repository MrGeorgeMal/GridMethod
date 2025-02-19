#include "Rasterizer.h"



// Rasterize shape 2D
Matrix2D<Material> Rasterizer::rasterize(Shape2D& shape2d)
{
	if (shape2d.getType() == "Line2D")
	{
		Line2D* line2d = dynamic_cast<Line2D*>(&shape2d);
		return rasterizeLine(line2d);
	}

	else if (shape2d.getType() == "Polygon2D" || shape2d.getType() == "Rectangle2D")
	{
		Polygon2D* poligon2d = dynamic_cast<Polygon2D*>(&shape2d);
		return rasterizePolygon(poligon2d);
	}

	else
	{
		return Matrix2D<Material>();
	}
}



// Rasterize line shape
Matrix2D<Material> Rasterizer::rasterizeLine(Line2D* line2d)
{
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
Matrix2D<Material> Rasterizer::rasterizePolygon(Polygon2D* polygon2d)
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



// Rasterize line, Wu method
Matrix2D<Material> Rasterizer::drawLineWu(Line2D* line2d)
{
	return Matrix2D<Material>();
}



// Rasterize polygon, no antialiasing method
Matrix2D<Material> Rasterizer::drawPolygon(Polygon2D* polygon2d)
{
	return Matrix2D<Material>();
}



// Rasterize polygon, super sampled antialiasing method
Matrix2D<Material> Rasterizer::drawPolygonSSAA(Polygon2D* polygon2d)
{
	return Matrix2D<Material>();
}
