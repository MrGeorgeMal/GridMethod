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
	Size2D<int> matrixSize = discretizeSize(screen->getSize());
	return matrixSize;
}



// Rasterize shape 2D
const Matrix2D<Rasterizer::CellInfo> Rasterizer::rasterize(const Vector<Shape2D*>& shapes) const
{
	// Create matrix
	Size2D<int> matrixSize = defineMatrixSize(shapes);
	int rows = matrixSize.height + _screenBorder * 2;
	int cols = matrixSize.width + _screenBorder * 2;
	Matrix2D<CellInfo> matrix(rows, cols);

	// Fill matrix with screen material
	Rectangle2D* screen = findScreen(shapes);
	Dielectric* fillMaterial = nullptr;

	if (findScreen(shapes)->getMaterial()->getType() == "Dielectric")
	{
		fillMaterial = dynamic_cast<Dielectric*>(screen->getMaterial());
	}

	if (fillMaterial == nullptr)
	{
		throw "Rasterizer::rasterize::Bad screen material";
		return matrix;
	}

	for (int y = 0; y < matrix.getRows(); y++)
	{
		for (int x = 0; x < matrix.getCols(); x++)
		{
			CellInfo cellInfo;
			cellInfo.dielectricValue = fillMaterial->getDielectricValue();
			cellInfo.potential = 0.0;
			cellInfo.isConductor = false;
		}
	}

	// Rasterize all dielectric shapes
	// it is necessary that cells with conductors also have information about the dielectric layer on which they lie
	for (int i = 0; i < shapes.getLength(); i++)
	{
		if (shapes[i]->getMaterial()->getType() == "Dielectric")
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

	return matrix;
}



// Plot matrix element (set cell info)
void Rasterizer::plot(Matrix2D<CellInfo>& matrix, int x, int y, Rasterizer::CellInfo cellInfo) const
{
	x += _screenBorder;
	y += _screenBorder;

	if (x >= 0 && x < matrix.getCols() - _screenBorder &&
		y >= 0 && y < matrix.getRows() - _screenBorder)
	{
		matrix[y][x] = cellInfo;
	}
}



// Rasterize line shape
void Rasterizer::rasterizeLine(const Line2D* line2d, Matrix2D<Rasterizer::CellInfo>& matrix) const
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
void Rasterizer::rasterizePolygon(const Polygon2D* polygon2d, Matrix2D<Rasterizer::CellInfo>& matrix) const
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
void Rasterizer::drawLineBresenham(const Line2D* line2d, Matrix2D<Rasterizer::CellInfo>& matrix) const
{

}



// Rasterize polygon, no antialiasing method
void Rasterizer::drawPolygon(const Polygon2D* polygon2d, Matrix2D<Rasterizer::CellInfo>& matrix) const
{

}



// Rasterize line, Wu method
void Rasterizer::drawLineWu(const Line2D* line2d, Matrix2D<Rasterizer::CellInfo>& matrix) const
{

}



// Rasterize polygon, super sampled antialiasing method
void Rasterizer::drawPolygonSSAA(const Polygon2D* polygon2d, Matrix2D<Rasterizer::CellInfo>& matrix) const
{

}