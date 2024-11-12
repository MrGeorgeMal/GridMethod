#pragma once
#include "Rectangle.h"

class Screen : public Rectangle
{
public:
	Screen() { _type = EStripObjectType::Screen; }

	void SetObject(Material newMaterial, double width, double height)
	{
		_material = newMaterial;

		if (width <= 0.0 || height <= 0.0)
		{
			throw "StripObject::Rectangle::BadSize";
			return;
		}

		_x = 0.0;
		_y = 0.0;
		_width = width;
		_height = height;
	}
};

