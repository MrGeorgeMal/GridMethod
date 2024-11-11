#pragma once
#include "StripObject.h"

class Line : public StripObject
{
private:
	double _x1 = 0.0;
	double _y1 = 0.0;
	double _x2 = 0.0;
	double _y2 = 0.0;

public:
	Line() {}

	void SetObject(Material newMaterial, double x1, double y1, double x2, double y2)
	{
		_material = newMaterial;

		double fixX1 = x1;
		double fixY1 = y1;
		double fixX2 = x2;
		double fixY2 = y2;

		if (x2 == x1 || y2 == y1)
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
};