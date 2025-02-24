#include "StripStructure.h"



// Compute electrostatic analysis
void StripStructure::computeElectroStaticAnalysis()
{
	Rectangle2D* screen = defineScreenRectangle();
}



// Find or create screen
Rectangle2D* StripStructure::defineScreenRectangle() const
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
		if (_shapes[i].getType() == "Rectangle2D")
		{
			Rectangle2D* rect = dynamic_cast<Rectangle2D*>(&_shapes[i]);
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
Rectangle2D* StripStructure::createScreenRectangle() const
{
	Rectangle2D* screen = new Rectangle2D();

	return screen;
}



// Get strip structure rect bound
Rect2D<double> StripStructure::getRectBound() const
{
	// left, top, right, bootom coordinates
	double l, t, r, b;

	return Rect2D<double>();
}



// Get offset shapes to center [0 ; 0]
const Vector<Shape2D>& StripStructure::getOffsetShapesToCenter() const
{
	return Vector<Shape2D>();
}
