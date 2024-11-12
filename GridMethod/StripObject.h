#pragma once
#include "Material.h"
#include <math.h>
#include <iostream>

class StripObject
{
protected:
	Material _material;
	int _fieldMatrixFragmentRows = 0;
	int _fieldMatrixFragmentCols = 0;
	Material **_fieldMatrixFragment = new Material*[_fieldMatrixFragmentRows];

public:
	void virtual GetFieldMatrixFragment(double dx, double dy) = 0;

	void virtual PrintFieldMatrixFragment() final
	{
		for (int i = 0; i < _fieldMatrixFragmentRows; i++)
		{
			for (int j = 0; j < _fieldMatrixFragmentCols; j++)
			{
				switch (_fieldMatrixFragment[i][j].materialType)
				{
				case Material::EMaterialType::None:
					std::cout << char(176);
					break;
				case Material::EMaterialType::Dielectric:
					std::cout << char(178);
					break;
				case Material::EMaterialType::Conductor:
					std::cout << char(219);
					break;
				default:
					break;
				}
			}
			std::cout << "\n";
		}
	}

protected:
	void virtual Disretization(double dx, double dy) = 0;

	void virtual UpdateFieldMatrixFragment(int rows, int cols) final
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

