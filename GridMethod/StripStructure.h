#pragma once
#include "StripObject.h"
#include "Screen.h"
#include "Rectangle.h"
#include "Line.h"

class StripStructure
{
private:
	int _objectsSize = 0;
	StripObject *_objects = 0;
	int _fieldMatrixRows = 0;
	int _fieldMatrixCols = 0;
	Material** _fieldMatrix = new Material * [_fieldMatrixRows];

public:
	StripStructure() {}

	void AddObject(StripObject &newObject)
	{
		_objects = (StripObject*)realloc(_objects, sizeof(StripObject) * (++_objectsSize));
		_objects[_objectsSize - 1] = newObject;
	}

	void UpdateFieldMatrix()
	{
		for (int i = 0; i < _objectsSize; i++)
		{
			
		}
	}

private:
	void UpdateFieldMatrix(int rows, int cols)
	{
		for (int i = 0; i < _fieldMatrixRows; i++)
		{
			delete[] _fieldMatrix[i];
		}
		delete[] _fieldMatrix;

		_fieldMatrixRows = rows;
		_fieldMatrixCols = cols;

		_fieldMatrix = new Material * [_fieldMatrixRows];
		for (int i = 0; i < _fieldMatrixRows; i++)
		{
			_fieldMatrix[i] = new Material[_fieldMatrixCols];
			for (int j = 0; j < _fieldMatrixCols; j++)
			{
				Material initMaterial;
				initMaterial.materialType = Material::EMaterialType::None;
				_fieldMatrix[i][j] = initMaterial;
			}
		}
	}
};