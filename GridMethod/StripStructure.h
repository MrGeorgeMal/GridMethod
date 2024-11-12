#pragma once
#include "StripObject.h"
#include "Screen.h"
#include "Rectangle.h"
#include "Line.h"
#include <vector>

class StripStructure
{
private:
	std::vector<StripObject*> _objects = std::vector<StripObject*>(0);
	int _fieldMatrixRows = 0;
	int _fieldMatrixCols = 0;
	Material** _fieldMatrix = new Material * [_fieldMatrixRows];

public:
	StripStructure() {}

	void AddObject(StripObject* newObject)
	{
		_objects.push_back(newObject);
	}

	void UpdateFieldMatrix(double dx, double dy)
	{
		Screen *screen = nullptr;

		for (int i = 0; i < _objects.size(); i++)
		{
			if (_objects[i]->GetType() == StripObject::EStripObjectType::Screen)
			{
				if (screen != nullptr)
				{
					throw "StripStructure::MultipleScreenError";
					return;
				}
				screen = dynamic_cast<Screen*>(_objects[i]);
			}
		}

		if (screen == nullptr)
		{
			throw "StripStructure::LackOfScreenError";
		}

		for (int i = 0; i < _objects.size(); i++)
		{
			_objects[i]->Rasterize(dx, dy);
		}

		UpdateFieldMatrix(screen->GetFieldMatrixFragmentRows(), screen->GetFieldMatrixFragmentCols());


		for (int i = 0; i < _objects.size(); i++)
		{
			int x1 = 0;
			int y1 = 0;
			int x2 = 0;
			int y2 = 0;
			switch (_objects[i]->GetType())
			{
			case StripObject::EStripObjectType::Screen:
			{
				Rectangle* screen;
				screen = dynamic_cast<Rectangle*>(_objects[i]);
				x1 = screen->GetDisX();
				y1 = screen->GetDisY();
				x2 = screen->GetDisX() + screen->GetDisWidth();
				y2 = screen->GetDisY() + screen->GetDisHeight();
				PasteFragmentToFieldMatrix(screen, x1, y1, x2, y2);
			}
				break;

			case StripObject::EStripObjectType::Rectangle:
			{
				Rectangle* rect;
				rect = dynamic_cast<Rectangle*>(_objects[i]);
				x1 = rect->GetDisX();
				y1 = rect->GetDisY();
				x2 = rect->GetDisX() + rect->GetDisWidth();
				y2 = rect->GetDisY() + rect->GetDisHeight();
				PasteFragmentToFieldMatrix(rect, x1, y1, x2, y2);
			}
				break;
			case StripObject::EStripObjectType::Line:
			{
				Line* line;
				line = dynamic_cast<Line*>(_objects[i]);
				x1 = line->GetDisX1();
				y1 = line->GetDisY1();
				x2 = line->GetDisX2();
				y2 = line->GetDisY2();
				PasteFragmentToFieldMatrix(line, x1, y1, x2, y2);
			}
			break;
			}
		}
	}

	void PrintStripStructure()
	{
		for (int i = 0; i < _fieldMatrixRows; i++)
		{
			for (int j = 0; j < _fieldMatrixCols; j++)
			{
				switch (_fieldMatrix[i][j].materialType)
				{
				case Material::EMaterialType::None:
					std::cout << char(255);
					break;
				case Material::EMaterialType::Dielectric:
					if (_fieldMatrix[i][j].dielectricValue > 1.0) std::cout << char(177);
					else std::cout << char(176);
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

	void PasteFragmentToFieldMatrix(StripObject* object, int x1, int y1, int x2, int y2)
	{
		if (x1 > _fieldMatrixCols || y1 > _fieldMatrixRows || x2 > _fieldMatrixCols || y2 > _fieldMatrixRows)
		{
			throw "StripStructure::BadStripObjectSize[Off-Screen]";
		}

		int objy = 0, objx = 0;
		for (int y = y1; y < y2; y++)
		{
			for (int x = x1; x < x2; x++)
			{
				if (object->GetFieldMatrixFragment()[objy][objx].materialType != Material::EMaterialType::None)
				{
					_fieldMatrix[y][x] = object->GetFieldMatrixFragment()[objy][objx];
				}
				objx++;
				if (objx >= object->GetFieldMatrixFragmentCols()) break;
			}
			objy++;
			if (objy >= object->GetFieldMatrixFragmentRows()) break;
			objx = 0;
		}
	}
};