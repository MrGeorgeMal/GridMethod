#pragma once
#include "StripObject.h"
#include "Screen.h"
#include "Rectangle.h"
#include "Line.h"
#include <vector>
#include <fstream>
#include "string"

class StripStructure
{
public:
	struct Area
	{
		Material material;
		double x1 = 0.0;
		double y1 = 0.0;
		double x2 = 0.0;
		double y2 = 0.0;
		int disX1 = 0;
		int disY1 = 0;
		int disX2 = 0;
		int disY2 = 0;
		int startDisY = 0;
		int startDisX = 0;
		double lengthToNearSignalConductor = 0.0;
	};

	struct DiscretePoint
	{
		int x = 0;
		int y = 0;
	};

private:
	std::vector<StripObject*> _objects = std::vector<StripObject*>(0);
	std::vector<Area> _areas = std::vector<Area>(0);
	std::vector<std::vector<DiscretePoint>> _signalConductorsPoints = std::vector<std::vector<DiscretePoint>>(0);

	int _fieldMatrixRows = 0;
	int _fieldMatrixCols = 0;
	Material** _fieldMatrix = new Material * [_fieldMatrixRows];
	
	double _minSizeX = 1.0;
	double _minSizeY = 1.0;

	std::vector<double> _energy = std::vector<double>(0);
	std::vector<double> _capacityPartialDielectric = std::vector<double>(0);
	std::vector<double> _capacityPartialAir = std::vector<double>(0);
	std::vector<std::vector<double>> _linearCapacitties = std::vector<std::vector<double>>(0);
	std::vector<std::vector<double>> _linearInductance = std::vector<std::vector<double>>(0);
	std::vector<double> _effectivePermettivity = std::vector<double>(0);
	std::vector<double> _waveImpedance = std::vector<double>(0);
	double _characteristicImpedance = 0.0;

public:
	StripStructure() {}

	~StripStructure()
	{
		/*
		if (_fieldMatrix != nullptr)
		{
			for (int i = 0; i < _fieldMatrixRows; i++)
			{
				if (_fieldMatrix[i] != nullptr) delete[] _fieldMatrix[i];
			}
			delete[] _fieldMatrix;
		}
		*/

		for (int i = 0; i < _objects.size(); i++)
		{
			delete _objects[i];
		}
	}

	void AddObject(StripObject* newObject)
	{
		_objects.push_back(newObject);
	}

	void BuildFieldMatrix(double dx, double dy)
	{
		DivideIntoAreas();
		RasterizeAndPastAllStripObjects(dx, dy);
		DisretizeAreas(dx, dy);
		SetStartPointForAreas();
		SortAreas();
		FindSignalConductorsPoints();
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
				case Material::EMaterialType::SignalConductor:
					std::cout << char(219);
					break;
				case Material::EMaterialType::ScreenConductor:
					std::cout << char(219);
					break;
				default:
					break;
				}
			}
			std::cout << "\n";
		}
	}

	void PrintStructureInfo()
	{
		std::cout << "\nSize info:\n";
		std::cout << "Cols and Rows: [" << _fieldMatrixRows << " ; " << _fieldMatrixCols << "]\n";
		std::cout << "Minimum size by X: " << _minSizeX << "\n";
		std::cout << "Minimum size by Y: " << _minSizeY << "\n";

		std::cout << "\nAreas info:\n";
		for (int i = 0; i < _areas.size(); i++)
		{
			std::cout << "Area " << i << " info:\n";
			std::cout << "\tDiscterized Coordinate: [" << _areas[i].disX1 << " ; " << _areas[i].disY1 << "] ";
			std::cout << ", [" << _areas[i].disX2 << " ; " << _areas[i].disY2 << "]\n";
			std::cout << "\tStart Point: [" << _areas[i].startDisX << " ; " << _areas[i].startDisY << "]\n";
		}

		std::cout << "\nSignal conductors info:\n";
		std::cout << "Signal conductors count: " << _signalConductorsPoints.size() << "\n";

		std::cout << "\nEnergy:\n";
		for (int i = 0; i < _energy.size(); i++)
		{
			std::cout << "Energy " << i << ": " << _energy[i] << "\n";
		}
	}

	void SaveFieldMatrixToFile(std::string fileName)
	{
		std::string fixedName = fileName;

		std::ifstream checkExist;
		int sameNameNumber = 0;
		checkExist.open(fileName);
		while (checkExist.is_open())
		{
			checkExist.close();
			sameNameNumber++;
			fixedName = fileName;
			int index = fileName.rfind('.');
			fixedName.insert(index, "_" + std::to_string(sameNameNumber));
			checkExist.open(fixedName);
		}

		std::ofstream file;
		file.open(fixedName);
		if (!file.is_open())
		{
			throw "StripStructure::CannotOpenFile";
			return;
		}
		
		for (int y = 0; y < _fieldMatrixRows; y++)
		{
			for (int x = 0; x < _fieldMatrixCols; x++)
			{
				file << _fieldMatrix[y][x].potentialValue;
				if (x < _fieldMatrixCols - 1) file << ",";
			}
			file << "\n";
		}
	}

	void SaveStructure(std::string fileName)
	{
		std::string fixedName = fileName;

		std::ifstream checkExist;
		int sameNameNumber = 0;
		checkExist.open(fileName);
		while (checkExist.is_open())
		{
			checkExist.close();
			sameNameNumber++;
			fixedName = fileName;
			int index = fileName.rfind('.');
			fixedName.insert(index, "_" + std::to_string(sameNameNumber));
			checkExist.open(fixedName);
		}

		std::ofstream file;
		file.open(fixedName);
		if (!file.is_open())
		{
			throw "StripStructure::CannotOpenFile";
			return;
		}

		for (int y = 0; y < _fieldMatrixRows; y++)
		{
			for (int x = 0; x < _fieldMatrixCols; x++)
			{
				switch (_fieldMatrix[y][x].materialType)
				{
				case Material::EMaterialType::Dielectric:
					file << 0 << ";";
					break;
				case Material::EMaterialType::SignalConductor:
					file << 1 << ";";
					break;
				case Material::EMaterialType::ScreenConductor:
					file << 2 << ";";
					break;
				default:
					break;
				}
				file << _fieldMatrix[y][x].dielectricValue;
				if (x < _fieldMatrixCols - 1) file << ",";
			}
			file << "\n";
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

	void RasterizeAndPastAllStripObjects(double dx, double dy)
	{
		bool haveSignalConductor = false;
		for (int i = 0; i < _objects.size(); i++)
		{
			_objects[i]->Rasterize(dx, dy);
			if (_objects[i]->GetMaterial().materialType == Material::EMaterialType::SignalConductor)
			{
				haveSignalConductor = true;
			}
		}

		if (!haveSignalConductor)
		{
			throw "StripStructure::BadCountSignalConductor[No-Conductors]";
		}

		UpdateFieldMatrix(GetScreen()->GetFieldMatrixFragmentRows(), GetScreen()->GetFieldMatrixFragmentCols());

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

				switch (line->align)
				{
				case Line::ELineAlign::LeftTop:
					if (x1 > 0 && x2 > 0)
					{
						x1--;
						x2--;
					}
					break;
				case Line::ELineAlign::LeftBottom:
					if (x1 > 0 && x2 > 0 && y1 > 0 && y2 > 0)
					{
						x1--;
						x2--;
						y1--;
						y2--;
					}
					break;
				case Line::ELineAlign::RightBottom:
					if (y1 > 0 && y2 > 0)
					{
						y1--;
						y2--;
					}
					break;
				default:
					break;
				}

				PasteFragmentToFieldMatrix(line, x1, y1, x2, y2);
			}
			break;
			}
		}
	}

	void DivideIntoAreas()
	{
		Screen* screen = GetScreen();

		// Scan structure by X coordinate
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

		// Scan structure by Y coordinate
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

		// Find the smallest dimension on the X axis
		_minSizeX = x[1] - x[0];
		if (x.size() > 2)
		{
			for (int i = 1; i < x.size() - 1; i++)
			{
				if (x[i + 1] - x[i] < _minSizeX) {
					_minSizeX = x[i + 1] - x[i];
				}
			}
		}

		// Find the smallest dimension on the Y axis
		_minSizeY = y[1] - y[0];
		if (y.size() > 2)
		{
			for (int i = 1; i < y.size() - 1; i++)
			{
				if (y[i + 1] - y[i] < _minSizeY) {
					_minSizeY = y[i + 1] - y[i];
				}
			}
		}

		// Build areas from scaned X and Y
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

		// Removing superfluous areas (combining areas with the same material that are close together)
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
	}

	void SetStartPointForAreas()
	{
		for (int i = 0; i < _areas.size(); i++)
		{
			// Determining if there are signal conductors in the area
			bool isAreaContainSignalConductor = false;
			for (int y = _areas[i].disY1; y < _areas[i].disY2; y++)
			{
				for (int x = _areas[i].disX1; x < _areas[i].disX2; x++)
				{
					if (_fieldMatrix[y][x].materialType == Material::EMaterialType::SignalConductor)
					{
						isAreaContainSignalConductor = true;
						break;
					}
				}
				if (isAreaContainSignalConductor) break;
			}

			// If this area contains signal conductors, then find a special point
			if (isAreaContainSignalConductor)
			{
				// Finding the sum for each row and column.
				// The intersection of the row with the highest sum and the column with the highest sum gives a special point
				std::vector<int> rowsSumm = std::vector<int>(0);
				std::vector<int> colsSumm = std::vector<int>(0);
				int maxRowSumm = 0;
				int maxColSumm = 0;

				for (int y = _areas[i].disY1; y < _areas[i].disY2; y++)
				{
					rowsSumm.push_back(0);
					for (int x = _areas[i].disX1; x < _areas[i].disX2; x++)
					{
						if (_fieldMatrix[y][x].materialType == Material::EMaterialType::SignalConductor)
						{
							rowsSumm[rowsSumm.size() - 1]++;
							
							if (maxRowSumm < rowsSumm[rowsSumm.size() - 1])
							{
								maxRowSumm = rowsSumm[rowsSumm.size() - 1];
							}
						}
					}
				}
				for (int x = _areas[i].disX1; x < _areas[i].disX2; x++)
				{
					colsSumm.push_back(0);
					for (int y = _areas[i].disY1; y < _areas[i].disY2; y++)
					{
						if (_fieldMatrix[y][x].materialType == Material::EMaterialType::SignalConductor)
						{
							colsSumm[colsSumm.size() - 1]++;

							if (maxColSumm < colsSumm[colsSumm.size() - 1])
							{
								maxColSumm = colsSumm[colsSumm.size() - 1];
							}
						}
					}
				}

				// Finding special points. There can be more than one
				std::vector<int> specialPointY = std::vector<int>();
				std::vector<int> specialPointX = std::vector<int>();
				for (int ri = 0; ri < rowsSumm.size(); ri++)
				{
					if (rowsSumm[ri] == maxRowSumm)
					{
						for (int ci = 0; ci < colsSumm.size(); ci++)
						{
							if (colsSumm[ci] == maxColSumm)
							{
								specialPointY.push_back(ri);
								specialPointX.push_back(ci);
							}
						}
					}
				}

				// If there are several special points, describe a square region through them and find its center.
				// The center of the square region will be a special point.
				if (specialPointX.size() > 1)
				{
					int deltax = floor( (specialPointX[specialPointX.size() - 1] - specialPointX[0]) / 2);
					int deltay = floor ( (specialPointY[specialPointY.size() - 1] - specialPointY[0]) / 2);
					_areas[i].startDisX = _areas[i].disX1 + specialPointX[0] + deltax;
					_areas[i].startDisY = _areas[i].disY1 + specialPointY[0] + deltay;
				}
				else
				{
					_areas[i].startDisX = _areas[i].disX1 + specialPointX[0];
					_areas[i].startDisY = _areas[i].disY1 + specialPointY[0];
				}

				_areas[i].lengthToNearSignalConductor = 0.0;
			}

			// If the area does not contain signal conductors,
			// the minimum length of each vertex of the area to the signal conductors is determined.
			// The vertex that is closest to the signal conductors is set as the starting point
			else
			{
				// Finding the coordinates of the points that correspond to the material of the signal conductor
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
				double deltax = _areas[i].disX1 - conductorX[0];
				double deltay = _areas[i].disY1 - conductorY[0];
				double minLength = sqrt((deltax * deltax) + (deltay * deltay));
				minLength = floor(minLength * 1000) / 1000;
				_areas[i].startDisX = _areas[i].disX1;
				_areas[i].startDisY = _areas[i].disY1;

				for (int j = 0; j < conductorY.size(); j++)
				{
					deltax = _areas[i].disX1 - conductorX[j];
					deltay = _areas[i].disY1 + (_areas[i].disY2 - _areas[i].disY1 - 1) - conductorY[j];
					double newMinLength = sqrt((deltax * deltax) + (deltay * deltay));
					newMinLength = floor(newMinLength * 1000) / 1000;
					if (newMinLength < minLength)
					{
						_areas[i].startDisX = _areas[i].disX1;
						_areas[i].startDisY = _areas[i].disY1 + (_areas[i].disY2 - _areas[i].disY1 - 1);
						minLength = newMinLength;
					}

					deltax = _areas[i].disX1 + (_areas[i].disX2 - _areas[i].disX1 - 1) - conductorX[j];
					deltay = _areas[i].disY1 + (_areas[i].disY2 - _areas[i].disY1 - 1) - conductorY[j];
					newMinLength = sqrt((deltax * deltax) + (deltay * deltay));
					newMinLength = floor(newMinLength * 1000) / 1000;
					if (newMinLength < minLength)
					{
						_areas[i].startDisX = _areas[i].disX1 + (_areas[i].disX2 - _areas[i].disX1 - 1);
						_areas[i].startDisY = _areas[i].disY1 + (_areas[i].disY2 - _areas[i].disY1 - 1);
						minLength = newMinLength;
					}

					deltax = _areas[i].disX1 + (_areas[i].disX2 - _areas[i].disX1 - 1) - conductorX[j];
					deltay = _areas[i].disY1 - conductorY[j];
					newMinLength = sqrt((deltax * deltax) + (deltay * deltay));
					newMinLength = floor(newMinLength * 1000) / 1000;
					if (newMinLength < minLength)
					{
						_areas[i].startDisX = _areas[i].disX1 + (_areas[i].disX2 - _areas[i].disX1 - 1);
						_areas[i].startDisY = _areas[i].disY1;
						minLength = newMinLength;
					}
				}
				_areas[i].lengthToNearSignalConductor = minLength;
			}
		}
	}

	void SortAreas()
	{
		if (_areas.size() > 1)
		{
			for (int i = 1; i < _areas.size(); i++)
			{
				bool isSorted = true;
				for (int j = 0; j < _areas.size() - i; j++)
				{
					if (_areas[j].lengthToNearSignalConductor > _areas[j + 1].lengthToNearSignalConductor)
					{
						Area tempArea = _areas[j];
						_areas[j] = _areas[j + 1];
						_areas[j + 1] = tempArea;
						isSorted = false;
					}
				}
				if (isSorted) break;
			}
		}
	}

	void FindSignalConductorsPoints()
	{
		for (int y = 0; y < _fieldMatrixRows; y++)
		{
			for (int x = 0; x < _fieldMatrixCols; x++)
			{
				if (_fieldMatrix[y][x].materialType == Material::EMaterialType::SignalConductor)
				{
					bool skipPoint = false;
					for (int i = 0; i < _signalConductorsPoints.size(); i++)
					{
						for (int j = 0; j < _signalConductorsPoints[i].size(); j++)
						{
							if (_signalConductorsPoints[i][j].x == x && _signalConductorsPoints[i][j].y == y)
							{
								skipPoint = true;
								break;
							}
						}
						if (skipPoint) break;
					}

					if (!skipPoint)
					{
						DiscretePoint newDiscretePoint;
						newDiscretePoint.x = x;
						newDiscretePoint.y = y;

						_signalConductorsPoints.push_back(std::vector<DiscretePoint>(0));
						_signalConductorsPoints[_signalConductorsPoints.size() - 1].push_back(newDiscretePoint);

						DetectAllSignalConductorPoint(x, y);
					}
				}
			}
		}
	}

	void DetectAllSignalConductorPoint(int x, int y)
	{
		if (y > 0)
		{
			DetectNextSignalConductorPoint(x, y - 1);
		}
		if (x > 0)
		{
			DetectNextSignalConductorPoint(x - 1, y);
		}
		if (y < _fieldMatrixRows - 1)
		{
			DetectNextSignalConductorPoint(x, y + 1);
		}
		if (x < _fieldMatrixCols - 1)
		{
			DetectNextSignalConductorPoint(x + 1, y);
		}
		if (y > 0 && x > 0)
		{
			DetectNextSignalConductorPoint(x - 1, y - 1);
		}
		if (y < _fieldMatrixRows - 1 && x > 0)
		{
			DetectNextSignalConductorPoint(x - 1, y + 1);
		}
		if (y < _fieldMatrixRows - 1 && x < _fieldMatrixCols - 1)
		{
			DetectNextSignalConductorPoint(x + 1, y + 1);
		}
		if (y > 0 && x < _fieldMatrixCols - 1)
		{
			DetectNextSignalConductorPoint(x + 1, y - 1);
		}
	}

	void DetectNextSignalConductorPoint(int x, int y)
	{
		if (_fieldMatrix[y][x].materialType == Material::EMaterialType::SignalConductor)
		{
			bool skipPoint = false;
			for (int i = _signalConductorsPoints[_signalConductorsPoints.size() - 1].size() - 1; i >= 0; i--)
			{
				if (_signalConductorsPoints[_signalConductorsPoints.size() - 1][i].x == x &&
					_signalConductorsPoints[_signalConductorsPoints.size() - 1][i].y == y)
				{
					skipPoint = true;
					break;
				}
			}

			if (!skipPoint)
			{
				DiscretePoint newDiscretePoint;
				newDiscretePoint.x = x;
				newDiscretePoint.y = y;

				_signalConductorsPoints[_signalConductorsPoints.size() - 1].push_back(newDiscretePoint);

				DetectAllSignalConductorPoint(x, y);
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

public:
	Material** GetFieldMatrix() { return _fieldMatrix; }

	int GetFieldMatrixRows() { return _fieldMatrixRows; }

	int GetFieldMatrixCols() { return _fieldMatrixCols; }

	std::vector<Area> GetAreas() { return _areas; }

	std::vector<std::vector<DiscretePoint>> GetSignalConductorsPoints() { return _signalConductorsPoints; }

	void AddEnergy(double value) { _energy.push_back(value); }

	void AddCapacityPartialDielectric(double newValue) { _capacityPartialDielectric.push_back(newValue); }

	void AddCapacityPartialAir(double newValue) { _capacityPartialAir.push_back(newValue); }
};