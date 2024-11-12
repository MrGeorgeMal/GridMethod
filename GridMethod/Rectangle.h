#pragma once
#include "StripObject.h"

class Rectangle : public StripObject
{
protected:
	double _x = 0.0;
	double _y = 0.0;
	double _width = 0.0;
	double _height = 0.0;

	int _disX = 0;
	int _disY = 0;
	int _disWidth = 0;
	int _disHeight = 0;

public:
	Rectangle() { _type = EStripObjectType::Rectangle; }

	void SetObject(Material newMaterial, double x, double y, double width, double height)
	{
		_material = newMaterial;

		if (x < 0.0 || y < 0.0 || width <= 0.0 || height <= 0.0)
		{
			throw "StripObject::Rectangle::BadSize";
			return;
		}

		_x = x;
		_y = y;
		_width = width;
		_height = height;
	}

	void virtual Rasterize(double dx, double dy) override
	{
		Disretize(dx, dy);

		UpdateFieldMatrixFragment(_disHeight, _disWidth);

		for (int i = 0; i < _fieldMatrixFragmentRows; i++)
		{
			for (int j = 0; j < _fieldMatrixFragmentCols; j++)
			{
				_fieldMatrixFragment[i][j] = _material;
			}
		}
	}

	int GetDisX() { return _disX; }
	int GetDisY() { return _disY; }
	int GetDisWidth() { return _disWidth; }
	int GetDisHeight() { return _disHeight; }

protected:
	void virtual Disretize(double dx, double dy) override
	{
		if (_width == 0.0 || _height == 0.0)
		{
			throw "StripObject::Rectangle::BadSize";
			return;
		}

		_disX = floor(_x / dx);
		_disY = floor(_y / dy);
		_disWidth = floor(_width / dx);
		_disHeight = floor(_height / dy);
	}
};

