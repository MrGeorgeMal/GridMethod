#pragma once
#include "StripObject.h"

class Rectangle : public StripObject
{
private:
	double _x = 0.0;
	double _y = 0.0;
	double _width = 0.0;
	double _height = 0.0;

	int _disX = 0;
	int _disY = 0;
	int _disWidth = 0;
	int _disHeight = 0;

public:
	Rectangle() {}

	void SetObject(Material newMaterial, double x, double y, double width, double height)
	{
		_material = newMaterial;

		double fixX = x;
		double fixY = y;
		double fixWidth = width;
		double fixHeight = height;

		if (width == 0.0 || height == 0.0)
		{
			throw "StripObject::Rectangle::BadSize";
			return;
		}
		if (width < 0)
		{
			fixX = width + x;
			fixWidth = -1.0 * width;
		}
		if (height < 0)
		{
			fixY = height + y;
			fixHeight = -1.0 * height;
		}

		_x = fixX;
		_y = fixY;
		_width = fixWidth;
		_height = fixHeight;
	}

	void virtual GetFieldMatrixFragment(double dx, double dy) override
	{
		Disretization(dx, dy);

		for (int i = 0; i < _fieldMatrixFragmentRows; i++)
		{
			delete[] _fieldMatrixFragment[i];
		}
		delete[] _fieldMatrixFragment;

		_fieldMatrixFragmentRows = _disHeight;
		_fieldMatrixFragmentCols = _disWidth;

		_fieldMatrixFragment = new Material*[_fieldMatrixFragmentRows];
		for (int i = 0; i < _fieldMatrixFragmentRows; i++)
		{
			_fieldMatrixFragment[i] = new Material[_fieldMatrixFragmentCols];
			for (int j = 0; j < _fieldMatrixFragmentCols; j++)
			{
				_fieldMatrixFragment[i][j] = _material;
			}
		}
	}

protected:
	void virtual Disretization(double dx, double dy) override
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

