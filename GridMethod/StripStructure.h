#pragma once
#include "StripObject.h"
#include "Screen.h"
#include "Rectangle.h"
#include "Line.h"
#include <vector>

class StripStructure
{
	struct Area
	{
		Material material;
		double x1;
		double y1;
		double x2;
		double y2;
		int disX1;
		int disY1;
		int disX2;
		int disY2;
		double startDisY;
		double startDisX;
	};

private:
	std::vector<StripObject*> _objects = std::vector<StripObject*>(0);
	std::vector<Area> _areas = std::vector<Area>(0);

	int _fieldMatrixRows = 0;
	int _fieldMatrixCols = 0;
	Material** _fieldMatrix = new Material * [_fieldMatrixRows];

	int _transferMatrixSize = 0;
	double* _capacityPartialDielectric = new double[_transferMatrixSize];
	double* _capacityPartialAir = new double[_transferMatrixSize];
	double** _capacityTransfer = new double*[_transferMatrixSize];
	double** _inductanceTransfer = new double*[_transferMatrixSize];
	double* _effectivePermettivity = new double[_transferMatrixSize];
	double* _waveImpedance = new double[_transferMatrixSize];
	double _characteristicImpedance = 0.0;

public:
	StripStructure() {}

	void AddObject(StripObject* newObject)
	{
		_objects.push_back(newObject);
	}

	void BuildFieldMatrix(double dx, double dy)
	{
		Screen* screen = GetScreen();

		DivideIntoAreas();
		DisretizeAreas(dx, dy);
		
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
				std::cout << "Screen discret size: [" << x1 << " ; " << y1 << "] [" << x2 << " ; " << y2 << "]\n";
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
				
				std::cout << "Dielectric discret size: [" << x1 << " ; " << y1 << "] [" << x2 << " ; " << y2 << "]\n";

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

				std::cout << "Conductor discret size: [" << x1 << " ; " << y1 << "] [" << x2 << " ; " << y2 << "]\n";

				PasteFragmentToFieldMatrix(line, x1, y1, x2, y2);
			}
			break;
			}
		}
	}

	void PrintStripStructure()
	{
		for (int i = _fieldMatrixRows - 1; i >= 0; i--)
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
				case Material::EMaterialType::SignalConductor | Material::EMaterialType::ScreenConductor:
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

	void DivideIntoAreas()
	{
		Screen* screen = GetScreen();

		std::vector<double> x = std::vector<double>(0);
		x.push_back(0.0);
		bool findNewX = true;
		while (findNewX == true)
		{
			double minX = screen->GetWidth();
			findNewX = false;

			for (int i = 0; i < _objects.size(); i++)
			{
				if (_objects[i]->GetType() == StripObject::EStripObjectType::Rectangle)
				{
					Rectangle* rect = dynamic_cast<Rectangle*>(_objects[i]);
					if (rect->GetX() < minX && rect->GetX() > x[x.size() - 1])
					{
						minX = rect->GetX();
						findNewX = true;
					}

					if (rect->GetX() + rect->GetWidth() < minX && rect->GetX() + rect->GetWidth() > x[x.size() - 1])
					{
						minX = rect->GetX() + rect->GetWidth();
						findNewX = true;
					}
				}

				if (_objects[i]->GetType() == StripObject::EStripObjectType::Line)
				{
					Line* line = dynamic_cast<Line*>(_objects[i]);
					if (line->GetX1() < minX && line->GetX1() > x[x.size() - 1])
					{
						minX = line->GetX1();
						findNewX = true;
					}

					if (line->GetX2() < minX && line->GetX2() > x[x.size() - 1])
					{
						minX = line->GetX2();
						findNewX = true;
					}
				}
			}
			x.push_back(minX);
		}

		std::vector<double> y = std::vector<double>(0);
		y.push_back(0.0);
		bool findNewY = true;
		while (findNewY == true)
		{
			double minY = screen->GetHeight();
			findNewY = false;

			for (int i = 0; i < _objects.size(); i++)
			{
				if (_objects[i]->GetType() == StripObject::EStripObjectType::Rectangle)
				{
					Rectangle* rect = dynamic_cast<Rectangle*>(_objects[i]);
					if (rect->GetY() < minY && rect->GetY() > y[y.size() - 1])
					{
						minY = rect->GetY();
						findNewY = true;
					}

					if (rect->GetY() + rect->GetHeight() < minY && rect->GetY() + rect->GetHeight() > y[y.size() - 1])
					{
						minY = rect->GetY() + rect->GetHeight();
						findNewY = true;
					}
				}

				if (_objects[i]->GetType() == StripObject::EStripObjectType::Line)
				{
					Line* line = dynamic_cast<Line*>(_objects[i]);
					if (line->GetY1() < minY && line->GetY1() > y[y.size() - 1])
					{
						minY = line->GetY1();
						findNewY = true;
					}

					if (line->GetY2() < minY && line->GetY2() > y[y.size() - 1])
					{
						minY = line->GetY2();
						findNewY = true;
					}
				}
			}
			y.push_back(minY);
		}

		for (int i = 0; i < y.size() - 1; i++)
		{
			for (int j = 0; j < x.size() - 1; j++)
			{
				Area newArea;
				newArea.x1 = x[j];
				newArea.y1 = y[i];
				newArea.x2 = x[j + 1];
				newArea.y2 = y[i + 1];
				newArea.material = GetAreaMaterial(newArea);
				_areas.push_back(newArea);
			}
		}

		for (int i = 0; i < _areas.size() - 1; i++)
		{
			int j = i + 1;
			while (_areas[i].material.materialType == _areas[j].material.materialType &&
				_areas[i].material.dielectricValue == _areas[j].material.dielectricValue)
			{
				_areas[i].x2 = _areas[j].x2;
				_areas[i].y2 = _areas[j].y2;
				_areas.erase(_areas.begin() + j);
				if (i >= _areas.size() - 1) break;
			}
			if (i >= _areas.size() - 1) break;
		}

		std::cout << "Points scan X:\n";
		for (int i = 0; i < x.size(); i++)
		{
			std::cout << "x" << i + 1 << " = " << x[i] << " ; ";
		}
		std::cout << "\n";

		std::cout << "Points scan Y:\n";
		for (int i = 0; i < y.size(); i++)
		{
			std::cout << "y" << i + 1 << " = " << y[i] << " ; ";
		}
		std::cout << "\n\n";

		std::cout << "Areas:\n";
		for (int i = 0; i < _areas.size(); i++)
		{
			std::cout << "area" << i + 1 << ": [" << _areas[i].x1 << " ; " << _areas[i].y1 << "] ";
			std::cout << "[" << _areas[i].x2 << "; " << _areas[i].y2 << "]";
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	void DisretizeAreas(double dx, double dy)
	{
		for (int i = 0; i < _areas.size(); i++)
		{
			_areas[i].disX1 = floor(_areas[i].x1 / dx);
			_areas[i].disY1 = floor(_areas[i].y1 / dy);
			_areas[i].disX2 = floor(_areas[i].x2 / dx);
			_areas[i].disY2 = floor(_areas[i].y2 / dy);
		}

		std::cout << "Discretized Areas:\n";
		for (int i = 0; i < _areas.size(); i++)
		{
			std::cout << "area" << i + 1 << ": [" << _areas[i].disX1 << " ; " << _areas[i].disY1 << "] ";
			std::cout << "[" << _areas[i].disX2 << "; " << _areas[i].disY2 << "]";
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	void SortAreas()
	{
		std::vector<int> conductorX = std::vector<int>(0);
		std::vector<int> conductorY = std::vector<int>(0);

		for (int y = 0; y < _fieldMatrixRows; y++)
		{
			for (int x = 0; x < _fieldMatrixCols; x++)
			{
				if (_fieldMatrix[y][x].materialType == Material::EMaterialType::SignalConductor)
				{
					conductorY.push_back(y);
					conductorX.push_back(x);
				}
			}
		}


		// Finding the minimum length to the conductor
		double deltax = _areas[0].disX1 - conductorX[0];
		double deltay = _areas[0].disY1 - conductorY[0];
		double minLength = sqrt((deltax * deltax) + (deltay * deltay));
		minLength = floor(minLength * 1000) / 1000;

		for (int i = 0; i < _areas.size(); i++)
		{
			bool stopFindMinLength = false;

			for (int j = 0; j < conductorY.size(); j++)
			{
				for (int y = _areas[i].disY1; y < _areas[i].disY2; y++)
				{
					for (int x = _areas[i].disX1; x < _areas[i].disX2; x++)
					{
						if (y == conductorY[j] && x == conductorX[j])
						{
							minLength = 0.0;
							stopFindMinLength = true;
							break;
						}
						else
						{
							deltax = x - conductorX[j];
							deltay = y - conductorY[j];
							double newMinLength = sqrt((deltax * deltax) + (deltay * deltay));
							newMinLength = floor(newMinLength * 1000) / 1000;

							if (newMinLength < minLength)
							{
								minLength = newMinLength;
							}
						}
					}
					if (stopFindMinLength) break;
				}
				if (stopFindMinLength) break;
			}
		}

		// Finding startX and startY for Area
		for (int i = 0; i < _areas.size(); i++)
		{
			std::vector<int> nearestPointsY = std::vector<int>(0);
			std::vector<int> nearestPointsX = std::vector<int>(0);

			for (int j = 0; j < conductorY.size(); j++)
			{
				for (int y = _areas[i].disY1; y < _areas[i].disY2; y++)
				{
					for (int x = _areas[i].disX1; x < _areas[i].disX2; x++)
					{
						if (y == conductorY[j] && x == conductorX[j])
						{
							nearestPointsY.push_back(y);
							nearestPointsX.push_back(x);
						}
						else
						{
							deltax = x - conductorX[j];
							deltay = y - conductorY[j];
							double newMinLength = sqrt((deltax * deltax) + (deltay * deltay));
							newMinLength = floor(newMinLength * 1000) / 1000;

							if (newMinLength <= minLength)
							{
								nearestPointsY.push_back(y);
								nearestPointsX.push_back(x);
							}
						}
					}
				}
			}

			// If the conductor is not inside the area,
			// determine the best starting point by the square area formed by the nearest points to the conductor
			if (minLength > 0.0)
			{
				// Find left bottom and right top points, which form the square region of the nearest points
				int leftBoottomY = nearestPointsY[0];
				int leftBoottomX = nearestPointsX[0];
				int rightTopY = nearestPointsY[0];
				int rightTopX = nearestPointsX[0];

				for (int j = 0; j < nearestPointsY.size(); j++)
				{
					if (nearestPointsY[j] < leftBoottomY && nearestPointsX[j] < leftBoottomX)
					{
						leftBoottomY = nearestPointsY[j];
						leftBoottomX = nearestPointsX[j];
					}
					if (nearestPointsY[j] > rightTopY && nearestPointsX[j] > rightTopX)
					{
						rightTopY = nearestPointsY[j];
						rightTopX = nearestPointsX[j];
					}
				}

				int centerY = leftBoottomY + floor(abs(float(rightTopY) - float(leftBoottomY)) / 2);
				int centerX = leftBoottomX + floor(abs(float(rightTopX) - float(leftBoottomX)) / 2);
				_areas[i].startDisY = centerY;
				_areas[i].startDisX = centerX;
			}
			// Else, search for the point where the largest number of conductors is adjacent
			else
			{
				for (int j = 0; j < nearestPointsY.size(); j++)
				{
					int maxConductorNumber = 1;
					int conductorNumber = 1;
					int y = nearestPointsY[j];
					int x = nearestPointsY[j];
					_areas[i].startDisY = y;
					_areas[i].startDisX = x;

					if (y > 0)
					{
						if (_fieldMatrix[y - 1][x].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (y < _fieldMatrixRows - 1)
					{
						if (_fieldMatrix[y + 1][x].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (x > 0)
					{
						if (_fieldMatrix[y][x - 1].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (x < _fieldMatrixCols - 1)
					{
						if (_fieldMatrix[y][x + 1].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (y > 0 && x > 0)
					{
						if (_fieldMatrix[y - 1][x - 1].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (y < _fieldMatrixRows - 1 && x > 0)
					{
						if (_fieldMatrix[y + 1][x - 1].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (y > 0 && x < _fieldMatrixCols - 1)
					{
						if (_fieldMatrix[y - 1][x + 1].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}
					if (y < _fieldMatrixRows - 1 && x < _fieldMatrixCols - 1)
					{
						if (_fieldMatrix[y + 1][x + 1].materialType == Material::EMaterialType::SignalConductor)
						{
							conductorNumber++;
						}
					}

					if (conductorNumber > maxConductorNumber)
					{
						maxConductorNumber = conductorNumber;
						_areas[i].startDisY = y;
						_areas[i].startDisX = x;
					}
				}
			}
		}

		// Sort Areas in order of increasing length to conductors
		std::vector<Area> sortedAreas = std::vector<Area>(0);
		for (int i = 0; i < _areas.size(); i++)
		{
			deltax = _areas[0].startDisX - conductorX[0];
			deltay = _areas[0].startDisY - conductorY[0];
			minLength = sqrt((deltax * deltax) + (deltay * deltay));
			minLength = floor(minLength * 1000) / 1000;

			for (int j = 0; j < conductorY.size(); j++)
			{

			}
		}
	}

	Screen* GetScreen()
	{
		Screen* screen = nullptr;

		for (int i = 0; i < _objects.size(); i++)
		{
			if (_objects[i]->GetType() == StripObject::EStripObjectType::Screen)
			{
				if (screen != nullptr)
				{
					throw "StripStructure::MultipleScreenError";
					return nullptr;
				}
				screen = dynamic_cast<Screen*>(_objects[i]);
			}
		}

		if (screen == nullptr)
		{
			throw "StripStructure::LackOfScreenError";
			return nullptr;
		}

		return screen;
	}

	Material GetAreaMaterial(Area area)
	{
		for (int i = 0; i < _objects.size(); i++)
		{
			if (_objects[i]->GetType() == StripObject::EStripObjectType::Rectangle)
			{
				Rectangle* rect = dynamic_cast<Rectangle*>(_objects[i]);
				double x1 = rect->GetX();
				double y1 = rect->GetY();
				double x2 = rect->GetX() + rect->GetWidth();
				double y2 = rect->GetY() + rect->GetHeight();

				if (area.x1 >= x1 && area.x2 <= x2 &&
					area.y1 >= y1 && area.y2 <= y2)
				{
					return rect->GetMaterial();
				}
			}
		}
		return GetScreen()->GetMaterial();
	}
};