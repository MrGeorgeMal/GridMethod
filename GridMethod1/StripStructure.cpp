#include "StripStructure.h"



// Compute electrostatic analysis
void StripStructure::computeElectroStaticAnalysis()
{
	

	_screen = defineScreenRectangle();
	_shapes.addToHead(_screen);

	Vector<Shape2D*> offsetShapes = getOffsetShapesToCenter();

	_rasterizer->setCell(defineOptimalCellSize());
	Matrix2D<Types::CellInfo> matrix = _rasterizer->rasterize(offsetShapes);

	std::cout << "**********************************************************************************************************************************************************************\n";
	std::cout << "* > Strip structure info\n";
	std::cout << "**********************************************************************************************************************************************************************\n\n";

	printStructureInfo(matrix);
	std::cout << "\n\n";
	
	std::cout << "**********************************************************************************************************************************************************************\n";
	std::cout << "* > Discretized strip structure\n";
	std::cout << "**********************************************************************************************************************************************************************\n\n";

	drawStructure(matrix);

	Matrix2D<Types::LinearParameters> linearParamenetrs = _gridSolver->computeLinearParameters(matrix);
}



// Find or create screen
Rectangle2D* StripStructure::defineScreenRectangle()
{
	Rectangle2D* screen = findScreenRectangle();
	if (screen == nullptr)
	{
		screen = createScreenRectangle();
	}
	return screen;
}



// Try to find screen
Rectangle2D* StripStructure::findScreenRectangle() const
{
	Rectangle2D* screen = nullptr;
	int screenShapeNumber = 0;

	for (int i = 0; i < _shapes.getLength(); i++)
	{
		if (_shapes[i]->getType() == "Rectangle2D")
		{
			Rectangle2D* rect = dynamic_cast<Rectangle2D*>(_shapes[i]);
			if (rect->isScreen())
			{
				screenShapeNumber++;
				if (screenShapeNumber > 1)
				{
					throw "StripStructur::findScreenRectangle::Bad screen number";
					return nullptr;
				}
				screen = rect;
			}
		}
	}

	return screen;
}



// Create new screen if could not find it
Rectangle2D* StripStructure::createScreenRectangle()
{
	Vector<Shape2D*> signalConductorShapes;
	for (size_t i = 0; i < _shapes.getLength(); i++)
	{
		if (_shapes[i]->getMaterial()->getType() == "Conductor")
		{
			Conductor* condMaterial = dynamic_cast<Conductor*>(_shapes[i]->getMaterial());

			if (condMaterial->isSignal() == true)
			{
				signalConductorShapes.add(_shapes[i]->getCopy());
			}
		}
	}

	Rect2D<double> generalBound = getRectBound(_shapes);
	Rect2D<double> conductorBound = getRectBound(signalConductorShapes);
	Rect2D<double> screenBound;

	screenBound.left = conductorBound.left - _screenDistance;
	screenBound.top = conductorBound.top + _screenDistance;
	screenBound.right = conductorBound.right + _screenDistance;
	screenBound.bottom = conductorBound.bottom - _screenDistance;

	if (screenBound.left > generalBound.left)
	{
		screenBound.left = generalBound.left;
	}
	if (screenBound.top < generalBound.top)
	{
		screenBound.top = generalBound.top;
	}
	if (screenBound.right < generalBound.right)
	{
		screenBound.right = generalBound.right;
	}
	if (screenBound.bottom > generalBound.bottom)
	{
		screenBound.bottom = generalBound.bottom;
	}

	Point2D<double> point(screenBound.left, screenBound.bottom);
	Size2D<double> size;
	size.width = screenBound.right - screenBound.left;
	size.height = screenBound.top - screenBound.bottom;

	Rectangle2D* screen = new Rectangle2D(point, size);
	Dielectric* material = new Dielectric(1.0, 0.0);

	screen->setMaterial(material);
	screen->makeAsScreen(true);

	return screen;
}



// Get strip structure rect bound
// shapes - shapes around which the rect bound will be taken 
Rect2D<double> StripStructure::getRectBound(const Vector<Shape2D*>& shapes) const
{
	// left, top, right, bootom coordinates
	double l = 0.0;
	double t = 0.0;
	double r = 0.0;
	double b = 0.0;

	// Init left and bottom values
	if (shapes[0]->getType() == "Line2D")
	{
		Line2D* line = dynamic_cast<Line2D*>(shapes[0]);
		l = line->getP1().x;
		b = line->getP1().y;
	}

	if (shapes[0]->getType() == "Polygon2D" || shapes[0]->getType() == "Rectangle2D")
	{
		Polygon2D* polygon = dynamic_cast<Polygon2D*>(shapes[0]);
		l = polygon->getPoints()[0].x;
		b = polygon->getPoints()[0].y;
	}

	// Define left, top, right and bottom coordinates
	for (int i = 0; i < shapes.getLength(); i++)
	{
		if (shapes[i]->getType() == "Line2D")
		{
			Line2D* line = dynamic_cast<Line2D*>(shapes[i]);
			double x1 = line->getP1().x;
			double y1 = line->getP1().y;
			double x2 = line->getP2().x;
			double y2 = line->getP2().y;

			l = std::min(l, x1);
			l = std::min(l, x2);
			t = std::max(t, y1);
			t = std::max(t, y2);
			r = std::max(r, x1);
			r = std::max(r, x2);
			b = std::min(b, y1);
			b = std::min(b, y2);
		}

		if (shapes[i]->getType() == "Polygon2D" || shapes[i]->getType() == "Rectangle2D")
		{
			Polygon2D* polygon = dynamic_cast<Polygon2D*>(shapes[i]);
			for (int j = 0; j < polygon->getPoints().getLength(); j++)
			{
				double x = polygon->getPoints()[j].x;
				double y = polygon->getPoints()[j].y;

				l = std::min(l, x);
				t = std::max(t, y);
				r = std::max(r, x);
				b = std::min(b, y);
			}
		}
	}

	return Rect2D<double>(l, t, r, b);
}



// Get offset shapes to center [0 ; 0]
Vector<Shape2D*> StripStructure::getOffsetShapesToCenter() const
{
	Vector<Shape2D*> offsetShapes;
	Point2D<double> offsetPoint = Point2D<double>(0.0, 0.0) - _screen->getPoint();

	for (int i = 0; i < _shapes.getLength(); i++)
	{
		Shape2D* offsetShape = _shapes[i]->getCopy();
		offsetShape->moveOrigin(offsetPoint);
		offsetShapes.add(offsetShape);
	}

	return offsetShapes;
}



// Add shape
void StripStructure::addShape(Shape2D* shape)
{
	_shapes.add(shape);
}



// Define min structure size
Size2D<double> StripStructure::defineMinSize() const
{
	// Create a vector of all points of all shapes
	Vector<double> x;
	Vector<double> y;
	for (int i = 0; i < _shapes.getLength(); i++)
	{
		if (_shapes[i]->getType() == "Line2D")
		{
			Line2D* line = dynamic_cast<Line2D*>(_shapes[i]);
			x.add(line->getP1().x);
			x.add(line->getP2().x);
			y.add(line->getP1().y);
			y.add(line->getP2().y);
		}

		if (_shapes[i]->getType() == "Polygon2D" || _shapes[i]->getType() == "Rectangle2D")
		{
			Polygon2D* polygon = dynamic_cast<Polygon2D*>(_shapes[i]);
			for (int i = 0; i < polygon->getPoints().getLength(); i++)
			{
				x.add(polygon->getPoints()[i].x);
				y.add(polygon->getPoints()[i].y);
			}
		}
	}

	// Sort points in increasing order
	Tool::sort(x);
	Tool::sort(y);

	// Compute all length two points (for x and y coordinates)
	Vector<double> lengthx;
	Vector<double> lengthy;

	// Find minimum length between two points
	double mindx = abs(x[x.getLength() - 1] - x[0]);
	for (int i = 1; i < x.getLength() - 1; i++)
	{
		double temp = Tool::roundToDouble(x[i + 1] - x[i]);
		if (temp != 0.0)
		{
			lengthx.add(temp);
		}

		if (mindx > x[i + 1] - x[i] && Tool::roundToDouble(x[i + 1] - x[i]) != 0.0)
		{
			mindx = abs(x[i + 1] - x[i]);
		}
	}

	double mindy = abs(y[y.getLength() - 1] - y[0]);
	for (int i = 1; i < y.getLength() - 1; i++)
	{
		if (Tool::roundToDouble(y[i + 1] - y[i]) != 0.0)
		{
			lengthy.add(abs(y[i + 1] - y[i]));
		}

		if (mindy > y[i + 1] - y[i] && Tool::roundToDouble(y[i + 1] - y[i]) != 0.0)
		{
			mindy = abs(y[i + 1] - y[i]);
		}
	}

	double minx = Tool::roundToDouble(x[x.getLength() - 1] - x[0]);
	double miny = Tool::roundToDouble(y[y.getLength() - 1] - y[0]);

	for (int i = 1; i < x.getLength() - 1; i++)
	{
		double length = Tool::roundToDouble(x[i + 1] - x[i]);
		if (length != 0.0)
		{
			int digitsAfterDot = Tool::defineDigitsCountAfterDot(length);

			if (digitsAfterDot == 0)
			{
				if (minx > length)
				{
					minx = length;
				}
			}
			else
			{
				double temp = length * pow(10, digitsAfterDot - 1);

				if (minx > temp - floor(temp) && temp - floor(temp) != 0.0)
				{
					minx = temp - floor(temp);
					minx /= pow(10, digitsAfterDot - 1);
				}
			}
		}
	}
	for (int i = 1; i < y.getLength() - 1; i++)
	{
		double length = Tool::roundToDouble(y[i + 1] - y[i]);
		if (length != 0.0)
		{
			int digitsAfterDot = Tool::defineDigitsCountAfterDot(length);

			if (digitsAfterDot == 0)
			{
				if (miny > length)
				{
					miny = length;
				}
			}
			else
			{
				double temp = length * pow(10, digitsAfterDot - 1);

				if (miny > temp - floor(temp) && temp - floor(temp) != 0.0)
				{
					miny = temp - floor(temp);
					miny /= pow(10, digitsAfterDot - 1);
				}
			}
		}
	}

	std::cout << "\n\n" << minx << " ; " << miny << "\n\n";

	return Size2D<double>(mindx, mindy);
}



// Define optimal cell size
Size2D<double> StripStructure::defineOptimalCellSize() const
{
	// compute optimal cell size
	Size2D<double> optimalCellSize;
	optimalCellSize.width = _screen->getWidth() / (double)_optimalGridSize.width;
	optimalCellSize.height = _screen->getHeight() / (double)_optimalGridSize.height;

	// if the calculated cell size is larger than the minimum structure size,
	// so compute it based on the minimum structure size.
	// division by 2 is necessary to improve accuracy (especially if the minimum size is a gap between two figures)
	Size2D<double> minSize = defineMinSize();
	bool isTooBigSize = false;
	if (minSize.width / 2 < optimalCellSize.width)
	{
		optimalCellSize.width = minSize.width / 2;
		isTooBigSize = true;
	}
	if (minSize.height / 2 < optimalCellSize.height)
	{
		optimalCellSize.height = minSize.height / 2;
		isTooBigSize = true;
	}

	// if the cells are to be the same size, then equate the width and height.
	// if there were no problems with the size of the initially computed cell size,
	// it is better to choose the largest size, otherwise, the smallest
	if (_isRegularGrid)
	{
		double size = 1.0;
		if (isTooBigSize)
		{
			size = std::min(optimalCellSize.width, optimalCellSize.height);
		}
		else
		{
			size = std::max(optimalCellSize.width, optimalCellSize.height);
		}
		optimalCellSize.width = size;
		optimalCellSize.height = size;
	}

	return optimalCellSize;
}



// Print strip structure info
void StripStructure::printStructureInfo(const Matrix2D<Types::CellInfo>& matrix) const
{
	std::cout << "Min size  [x ; y]:\t" << defineMinSize() << "\n";
	std::cout << "Cell size [x ; y]:\t" << defineOptimalCellSize() << "\n";
	std::cout << "Grid size [x ; y]:\t[" << matrix.getCols() << " ; " << matrix.getRows() << "]";
}



// Draw strip structure
void StripStructure::drawStructure(const Matrix2D<Types::CellInfo>& matrix) const
{
	char air = '.';
	char dielectric = '#';
	char signalConductor = char(219);
	char groundConductor = char(177);

	std::cout << "\t";
	for (int x = 0; x < matrix.getCols(); x++)
	{
		if (x * 10 > matrix.getCols())
		{
			break;
		}

		std::cout << ">" << x * 10;

		int number = (x * 10);
		int numberLength = 1;

		while (number /= 10)
		{
			numberLength++;
		}

		for (int i = 0; i < 9; i++)
		{
			if (i < 9 - numberLength)
			{
				std::cout << " ";
			}
			else
			{
				break;
			}
		}
	}

	std::cout << "\n\t";
	int xi = 0;
	for (int x = 0; x < matrix.getCols(); x++)
	{
		if (xi > 0)
		{
			std::cout << xi;
		}
		else
		{
			std::cout << " ";
		}

		xi++;
		if (xi >= 10)
		{
			xi = 0;
		}
	}
	std::cout << "\n\n";

	for (int y = matrix.getRows() - 1; y >= 0; y--)
	{
		std::cout << y << "\t";
		for (int x = 0; x < matrix.getCols(); x++)
		{
			if (matrix[y][x].isConductor == false)
			{
				if (matrix[y][x].dielectricValue == 1.0)
				{
					std::cout << air;
				}
				else
				{
					std::cout << dielectric;
				}
			}
			else
			{
				if (matrix[y][x].isSignalConductor)
				{
					std::cout << signalConductor;
				}
				else
				{
					std::cout << groundConductor;
				}
			}
		}
		if (y > 0)
		{
			std::cout << "\n";
		}
	}
}
