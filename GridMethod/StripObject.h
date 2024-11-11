#pragma once
#include "Material.h"
#include <math.h>

class StripObject
{
protected:
	Material _material;
	int _fieldMatrixFragmentRows = 0;
	int _fieldMatrixFragmentCols = 0;
	Material **_fieldMatrixFragment = new Material*[_fieldMatrixFragmentRows];

public:
	void virtual GetFieldMatrixFragment(double dx, double dy) = 0;

protected:
	void virtual Disretization(double dx, double dy) = 0;
};

