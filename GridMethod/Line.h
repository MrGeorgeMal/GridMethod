#pragma once
#include "StripObject.h"

class Line : public StripObject
{
private:
	double _x1 = 0.0;
	double _y1 = 0.0;
	double _x2 = 0.0;
	double _y2 = 0.0;

	int _disX1 = 0;
	int _disY1 = 0;
	int _disX2 = 0;
	int _disY2 = 0;

public:
	Line() { _type = EStripObjectType::Line; }

	void SetObject(Material newMaterial, double x1, double y1, double x2, double y2)
	{
		_material = newMaterial;

		double fixX1 = x1;
		double fixY1 = y1;
		double fixX2 = x2;
		double fixY2 = y2;

		if (x1 < 0.0 || y1 < 0.0 || x2 < 0 || y2 < 0)
		{
			throw "StripObject::Line::BadSize";
			return;
		}
		if (x2 == x1 && y2 == y1)
		{
			throw "StripObject::Line::BadSize";
			return;
		}
		if (x2 < x1)
		{
			fixX1 = x2;
			fixX2 = x1;
		}
		if (y2 < y1)
		{
			fixY1 = y2;
			fixY2 = y1;
		}

		_x1 = fixX1;
		_y1 = fixY1;
		_x2 = fixX2;
		_y2 = fixY2;
	}

	void virtual Rasterize(double dx, double dy) override
	{
		Disretize(dx, dy);

		UpdateFieldMatrixFragment(_disY2 - _disY1, _disX2 - _disX1);

		bool steep = abs(_fieldMatrixFragmentRows) > abs(_fieldMatrixFragmentCols);
		int fixRows = _fieldMatrixFragmentRows;
		int fixCols = _fieldMatrixFragmentCols;
		if (steep)
		{
			fixRows = _fieldMatrixFragmentCols;
			fixCols = _fieldMatrixFragmentRows;
		}

		// Rasterization by Bresenham Algorithm
		int deltax = fixCols;
		int deltay = fixRows;
		int error = deltax / 2;
		int y = 0;
		for (int x = 0; x < fixCols; x++)
		{
			_fieldMatrixFragment[steep ? x : y][steep ? y : x] = _material;
			error -= deltay;
			if (error < 0)
			{
				if (y < fixRows - 1) y ++;
				error += deltax;
			}
		}
	}

	int GetDisX1() { return _disX1; }
	int GetDisY1() { return _disY1; }
	int GetDisX2() { return _disX2; }
	int GetDisY2() { return _disY2; }

protected:
	void virtual Disretize(double dx, double dy) override
	{
		if (_x1 == _x2 && _y1 == _y2)
		{
			throw "StripObject::Line::BadSize";
			return;
		}

		_disX1 = floor(_x1 / dx);
		_disY1 = floor(_y1 / dy);
		_disX2 = floor(_x2 / dx);
		_disY2 = floor(_y2 / dy);

		if (_disX1 == _disX2)
		{
			_disX2++;
		}
		if (_disY1 == _disY2)
		{
			_disY2++;
		}
	}
};