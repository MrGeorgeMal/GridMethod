#pragma once
#include "Material.h"
#include <math.h>
#include <iostream>

class StripObject
{
public:
	enum EStripObjectType
	{
		None,
		Screen,
		Rectangle,
		Line
	};

protected:
	EStripObjectType _type = EStripObjectType::None;
	Material _material;
	int _fieldMatrixFragmentRows = 0;
	int _fieldMatrixFragmentCols = 0;
	Material **_fieldMatrixFragment = new Material*[_fieldMatrixFragmentRows];

public:
	void virtual Rasterize(double dx, double dy) = 0;

	EStripObjectType GetType()
	{
		return _type;
	}

	int GetFieldMatrixFragmentRows()
	{
		return _fieldMatrixFragmentRows;
	}

	int GetFieldMatrixFragmentCols()
	{
		return _fieldMatrixFragmentCols;
	}

	Material** GetFieldMatrixFragment() { return _fieldMatrixFragment; }

	Material GetMaterial() { return _material; }

protected:
	void virtual Disretize(double dx, double dy) = 0;

	void UpdateFieldMatrixFragment(int rows, int cols)
	{
		for (int i = 0; i < _fieldMatrixFragmentRows; i++)
		{
			delete[] _fieldMatrixFragment[i];
		}
		delete[] _fieldMatrixFragment;

		_fieldMatrixFragmentRows = rows;
		_fieldMatrixFragmentCols = cols;

		_fieldMatrixFragment = new Material * [_fieldMatrixFragmentRows];
		for (int i = 0; i < _fieldMatrixFragmentRows; i++)
		{
			_fieldMatrixFragment[i] = new Material[_fieldMatrixFragmentCols];
			for (int j = 0; j < _fieldMatrixFragmentCols; j++)
			{
				Material initMaterial;
				initMaterial.materialType = Material::EMaterialType::None;
				_fieldMatrixFragment[i][j] = initMaterial;
			}
		}
	}
};

