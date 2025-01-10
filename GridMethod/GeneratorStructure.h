#pragma once
#include "StripStructure.h"
#include "Line.h"
#include "Screen.h"
#include "Rectangle.h"
#include <random>


static class GeneratorStructure
{
public:
	struct Point
	{
		double x;
		double y;
	};

	static std::vector<StripObject*> stripObjects;
	static double width, height;

	static void Generate(double screenWidth, double screenHeight)
	{
		for (int i = 0; i < stripObjects.size(); i++)
		{
			delete stripObjects[i];
		}
		stripObjects.clear();

		width = screenWidth;
		height = screenHeight;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distSignalConductorCount(1, 2);
		std::uniform_int_distribution<int> distScreenConductorCount(0, 2);
		std::uniform_int_distribution<int> distDielectricCount(0, 3);

		int signalConducorCount = distSignalConductorCount(gen);
		int screenConductorCount = distScreenConductorCount(gen);
		int dielectricCount = distDielectricCount(gen);

		Material matAir;
		Material matSignalConductor;
		Material matScreenConductor;
		matAir.materialType = Material::EMaterialType::Dielectric;
		matAir.dielectricValue = 1.0;
		matSignalConductor.materialType = Material::EMaterialType::SignalConductor;
		matScreenConductor.materialType = Material::EMaterialType::ScreenConductor;

		Screen* screen = new Screen();
		screen->SetObject(matAir, width, height);
		stripObjects.push_back(screen);

		for (int i = 0; i < dielectricCount; i++)
		{
			stripObjects.push_back(GenerateRectangle(GenerateDielectricMaterial()));
		}
		for (int i = 0; i < signalConducorCount; i++)
		{
			stripObjects.push_back(GenerateLine(matSignalConductor));
		}
		for (int i = 0; i < screenConductorCount; i++)
		{
			stripObjects.push_back(GenerateLine(matScreenConductor));
		}
	}

	static Point GeneratePoint(Point excludePoint = Point(), bool exclude = false, bool stayHorizontal = false, bool stayVertical = false)
	{
		if (stayHorizontal && stayVertical)
		{
			throw "GeneratorStructur::BadArguments";
		}

		Point p;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> distX(0.0, width);
		std::uniform_real_distribution<double> distY(0.0, height);

		if (stayHorizontal)
		{
			p.x = distX(gen);
			p.y = excludePoint.y;
		}
		else if (stayVertical)
		{
			p.x = excludePoint.x;
			p.y = distY(gen);
		}
		else
		{
			p.x = distX(gen);
			p.y = distY(gen);
		}

		if (exclude && p.x == excludePoint.x && p.y == excludePoint.y)
		{
			GeneratePoint(excludePoint, exclude, stayHorizontal, stayVertical);
		}
		else
		{
			return p;
		}
	}

	static Line* GenerateLine(Material material)
	{
		Point p1 = GeneratePoint();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distStayType(0, 1);
		int stayType = distStayType(gen);
		bool stayHorizontal = false;
		bool stayVertical = false;
		if (stayType == 0)
		{
			stayHorizontal = true;
			stayVertical = false;
		}
		else if (stayType == 1)
		{
			stayHorizontal = false;
			stayVertical = true;
		}
		else
		{
			stayHorizontal = false;
			stayVertical = false;
		}
		Point p2 = GeneratePoint(p1, true, stayHorizontal, stayVertical);

		Line* newLine = new Line();
		newLine->SetObject(material, p1.x, p1.y, p2.x, p2.y);
		return newLine;
	}

	static Rectangle* GenerateRectangle(Material material)
	{
		Point p = GeneratePoint();

		double minWidth = 0.1;
		double minHeight = 0.1;
		double maxWidth = width - p.x;
		double maxHeight = height - p.y;
		
		while (minWidth > maxWidth || minHeight > maxHeight)
		{
			p = GeneratePoint();
			maxWidth = width - p.x;
			maxHeight = height - p.y;
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> distWidth(minWidth, maxWidth);
		std::uniform_real_distribution<double> distHeight(minHeight, maxHeight);

		Rectangle *newRect = new Rectangle();
		newRect->SetObject(material, p.x, p.y, distWidth(gen), distHeight(gen));
		return newRect;
	}


	static Material GenerateDielectricMaterial()
	{
		Material matDielectric;
		matDielectric.materialType = Material::EMaterialType::Dielectric;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> distDielectricValue(1.0, 100.0);

		matDielectric.dielectricValue = distDielectricValue(gen);

		return matDielectric;
	}
};