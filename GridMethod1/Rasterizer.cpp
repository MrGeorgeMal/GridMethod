#include "Rasterizer.h"



// Find screen in shapes
Rectangle2D* Rasterizer::findScreen(const Vector<Shape2D*>& shapes) const
{
	Rectangle2D* screen = nullptr;
	for (int i = 0; i < shapes.getLength(); i++)
	{
		if (shapes[i]->getType() == "Rectangle2D")
		{
			Rectangle2D* rectangle = dynamic_cast<Rectangle2D*>(shapes[i]);
			if (rectangle->isScreen())
			{
				screen = rectangle;
				break;
			}
		}
	}
	return screen;
}


// Define matrix size
Size2D<int> Rasterizer::defineMatrixSize(const Vector<Shape2D*>& shapes) const
{
	Rectangle2D* screen = findScreen(shapes);
	Size2D<int> matrixSize = Tool::discretizeSize(screen->getSize(), _cell);
	return matrixSize;
}


// Get updated cell info based on old cell info and new material
Types::CellInfo Rasterizer::getUpdatedCellInfo(Types::CellInfo oldCellInfo, Material* material) const
{
	Types::CellInfo updatedCellInfo = oldCellInfo;

	if (material->getType() == "Dielectric")
	{
		Dielectric* dielectric = dynamic_cast<Dielectric*>(material);
		updatedCellInfo.dielectricValue = dielectric->getDielectricValue();
		updatedCellInfo.potential = 0.0;
		updatedCellInfo.isConductor = false;
	}
	else if (material->getType() == "Conductor")
	{
		Conductor* conductor = dynamic_cast<Conductor*>(material);
		updatedCellInfo.potential = conductor->isSignal() ? 1.0 : 0.0;
		updatedCellInfo.isConductor = true;
	}
	else
	{
		throw "Rasteriser::getUpdatedCellInfo::Bad material type";
	}

	return updatedCellInfo;
}



// Rasterize shape 2D
const Matrix2D<Types::CellInfo> Rasterizer::rasterize(const Vector<Shape2D*>& shapes) const
{
	// Create matrix
	Size2D<int> matrixSize = defineMatrixSize(shapes);
	int rows = matrixSize.height + _screenBorder * 2;
	int cols = matrixSize.width + _screenBorder * 2;
	Matrix2D<Types::CellInfo> matrix(rows, cols);

	// Fill matrix with screen material
	Rectangle2D* screen = findScreen(shapes);
	for (int y = 0; y < matrix.getRows(); y++)
	{
		for (int x = 0; x < matrix.getCols(); x++)
		{
			Types::CellInfo cellInfo = getUpdatedCellInfo(matrix[y][x], screen->getMaterial());
			matrix[y][x] = cellInfo;
		}
	}

	// Rasterize all dielectric shapes
	// it is necessary that cells with conductors also have information about the dielectric layer on which they lie
	for (int i = 0; i < shapes.getLength(); i++)
	{
		if (shapes[i] != findScreen(shapes) &&
			shapes[i]->getMaterial()->getType() == "Dielectric")
		{
			if (shapes[i]->getType() == "Line2D")
			{
				Line2D* line = dynamic_cast<Line2D*>(shapes[i]);
				rasterizeLine(line, matrix);
			}

			if (shapes[i]->getType() == "Polygon2D" || shapes[i]->getType() == "Rectangle2D")
			{
				Polygon2D* polygon = dynamic_cast<Polygon2D*>(shapes[i]);
				rasterizePolygon(polygon, matrix);
			}
		}
	}

	// Rasterize all shapes
	for (int i = 0; i < shapes.getLength(); i++)
	{
		if (shapes[i] != findScreen(shapes))
		{
			if (shapes[i]->getType() == "Line2D")
			{
				Line2D* line = dynamic_cast<Line2D*>(shapes[i]);
				rasterizeLine(line, matrix);
			}

			if (shapes[i]->getType() == "Polygon2D" || shapes[i]->getType() == "Rectangle2D")
			{
				Polygon2D* polygon = dynamic_cast<Polygon2D*>(shapes[i]);
				rasterizePolygon(polygon, matrix);
			}
		}
	}

	return matrix;
}



// Plot matrix element (set cell info)
void Rasterizer::plot(Matrix2D<Types::CellInfo>& matrix, int x, int y, Material* material) const
{
	x += _screenBorder;
	y += _screenBorder;

	if (x >= _screenBorder && x < matrix.getCols() - _screenBorder &&
		y >= _screenBorder && y < matrix.getRows() - _screenBorder)
	{
		Types::CellInfo cellInfo = getUpdatedCellInfo(matrix[y][x], material);
		matrix[y][x] = cellInfo;
	}
}



// Check point if it inside the polygon (ray casting)
bool Rasterizer::isInsidePolygon(int x, int y, const Vector<Point2D<double>>& polygonPoints) const
{
	bool isInside = false;

	// Use center of cell for check
	double cx = (double)x + 0.5;
	double cy = (double)y + 0.5;
	
	int j = polygonPoints.getLength() - 1;
	for (int i = 0; i < polygonPoints.getLength(); i++)
	{
		Point2D<int> pi = Tool::discretizePoint(polygonPoints[i], _cell);
		Point2D<int> pj = Tool::discretizePoint(polygonPoints[j], _cell);

		double xi = (double)pi.x;
		double yi = (double)pi.y;
		double xj = (double)pj.x;
		double yj = (double)pj.y;

		// true if the point [cx, cy] lies between yi and yj (if you draw a horizontal line to the points [cx, cy])
		bool isPointBetweenYiYj = (yi > cy) != (yj > cy);

		// compute x coordinate of intersection of segment coonecting [xi, yi] and [xj, yj] with horizontal line cy (linear interpolation)
		// 1e-12 - protection against division by zero
		double intersectionX = (xj - xi) * (cy - yi) / (yj - yi + 1e-12) + xi;

		// true if cx to the left of the compute intersectionX point
		bool isLeftOfIntesectionX = cx < intersectionX;

		// true if:
		// 1) the segment connecting (xi,yi) and (xj,yj) intersects a horizontal line passing through the point (cx,cy)
		// 2) the point of intersection of this segment with the cy line is to the right of (cx,cy)
		bool isIntersect = isPointBetweenYiYj && isLeftOfIntesectionX;

		// even / odd rule in the ray casting algorithm for determining whether a point belongs to a polygon
		if (isIntersect)
		{
			isInside = !isInside;
		}

		j = i;
	}

	return isInside;
}



// Rasterize line shape
void Rasterizer::rasterizeLine(const Line2D* line2d, Matrix2D<Types::CellInfo>& matrix) const
{
	if (_antialiasing)
	{
		drawLineWu(line2d, matrix);
	}
	else
	{
		drawLineBresenham(line2d, matrix);
	}
}



// Rasterize polygon shape
void Rasterizer::rasterizePolygon(const Polygon2D* polygon2d, Matrix2D<Types::CellInfo>& matrix) const
{
	if (_antialiasing)
	{
		drawPolygonSSAA(polygon2d, matrix);
	}
	else
	{
		drawPolygon(polygon2d, matrix);
	}
}



// Rasterize line, Bresenham method
void Rasterizer::drawLineBresenham(const Line2D* line2d, Matrix2D<Types::CellInfo>& matrix) const
{
	Point2D<int> p1 = Tool::discretizePoint(line2d->getP1(), _cell);
	Point2D<int> p2 = Tool::discretizePoint(line2d->getP2(), _cell);
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int dirx = x1 < x2 ? 1 : -1;
	int diry = y1 < y2 ? 1 : -1;
	int err = dx - dy;

	while (true)
	{
		if (x1 == x2 && y1 == y2) break;

		plot(matrix, x1, y1, line2d->getMaterial());
		
		int deltaerr = 2 * err;
		if (deltaerr > -dy)
		{
			err -= dy;
			x1 += dirx;
		}
		if (deltaerr < dx)
		{
			err += dx;
			y1 += diry;
		}
	}
}



// Rasterize polygon, no antialiasing method
void Rasterizer::drawPolygon(const Polygon2D* polygon2d, Matrix2D<Types::CellInfo>& matrix) const
{
	for (int y = 0; y < matrix.getRows(); y++)
	{
		for (int x = 0; x < matrix.getCols(); x++)
		{
			bool isInside = isInsidePolygon(x, y, polygon2d->getPoints());
			if (isInside)
			{
				plot(matrix, x, y, polygon2d->getMaterial());
			}
		}
	}
}



// Rasterize line, Wu method
void Rasterizer::drawLineWu(const Line2D* line2d, Matrix2D<Types::CellInfo>& matrix) const
{

}



// Rasterize polygon, super sampled antialiasing method
void Rasterizer::drawPolygonSSAA(const Polygon2D* polygon2d, Matrix2D<Types::CellInfo>& matrix) const
{

}