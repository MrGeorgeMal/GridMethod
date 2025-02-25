#include "StripStructure.h"



// Compute electrostatic analysis
void StripStructure::computeElectroStaticAnalysis()
{
	_screen = defineScreenRectangle();
	Vector<Shape2D*> offsetShapes = getOffsetShapesToCenter();
	Matrix2D<Rasterizer::CellInfo> matrix = _rasterizer->rasterize(offsetShapes);


	std::cout << *_screen << "\n\n";
	for (int i = 0; i < offsetShapes.getLength(); i++)
	{
		if (_shapes[i]->getType() == "Line2D")
		{
			Line2D* line = dynamic_cast<Line2D*>(_shapes[i]);
			std::cout << *line << "\n\n";
		}

		if (_shapes[i]->getType() == "Polygon2D")
		{
			Polygon2D* polygon = dynamic_cast<Polygon2D*>(_shapes[i]);
			std::cout << *polygon << "\n\n";
		}

		if (_shapes[i]->getType() == "Rectangle2D")
		{
			Rectangle2D* rectangle = dynamic_cast<Rectangle2D*>(_shapes[i]);
			std::cout << *rectangle << "\n\n";
		}
	}
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
	Rect2D<double> bound = getRectBound();

	Point2D<double> point;
	point.x = bound.left - _screenDistance;
	point.y = bound.bottom - _screenDistance;

	Size2D<double> size;
	size.width = bound.right - bound.left + _screenDistance * 2;
	size.height = bound.top - bound.bottom + _screenDistance * 2;

	Rectangle2D* screen = new Rectangle2D(point, size);
	Dielectric* material = new Dielectric(1.0);

	screen->setMaterial(material);
	screen->makeAsScreen(true);

	_shapes.addToHead(screen);

	return screen;
}



// Get strip structure rect bound
Rect2D<double> StripStructure::getRectBound() const
{
	// left, top, right, bootom coordinates
	double l = 0.0;
	double t = 0.0;
	double r = 0.0;
	double b = 0.0;

	// Init left and bottom values
	if (_shapes[0]->getType() == "Line2D")
	{
		Line2D* line = dynamic_cast<Line2D*>(_shapes[0]);
		l = line->getP1().x;
		b = line->getP1().y;
	}

	if (_shapes[0]->getType() == "Polygon2D" || _shapes[0]->getType() == "Rectangle2D")
	{
		Polygon2D* polygon = dynamic_cast<Polygon2D*>(_shapes[0]);
		l = polygon->getPoints()[0].x;
		b = polygon->getPoints()[0].y;
	}

	// Define left, top, right and bottom coordinates
	for (int i = 0; i < _shapes.getLength(); i++)
	{
		if (_shapes[i]->getType() == "Line2D")
		{
			Line2D* line = dynamic_cast<Line2D*>(_shapes[i]);
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

		if (_shapes[i]->getType() == "Polygon2D" || _shapes[i]->getType() == "Rectangle2D")
		{
			Polygon2D* polygon = dynamic_cast<Polygon2D*>(_shapes[i]);
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
const Vector<Shape2D*> StripStructure::getOffsetShapesToCenter() const
{
	Vector<Shape2D*> offsetShapes;
	Point2D<double> offsetPoint = Point2D<double>(0.0, 0.0) - _screen->getPoint();

	for (size_t i = 0; i < _shapes.getLength(); i++)
	{
		Shape2D* offsetShape(_shapes[i]);
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