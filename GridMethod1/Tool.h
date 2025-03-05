#pragma once
#ifndef TOOL_H
#define TOOL_H

#include "Vector.h"
#include "Point2D.h"
#include "Size2D.h"

static class Tool
{

public:

	// Swap two element
	static void swap(double& a, double& b)
	{
		double temp = a;
		a = b;
		b = temp;
	}
	

	// Bubble Sort
	static void sort(Vector<double>& vector)
	{
		for (int i = 0; i < vector.getLength() - 1; i++)
		{
			bool isSwapped = false;

			for (int j = 0; j < vector.getLength() - i - 1; j++)
			{
				if (vector[j] > vector[j + 1]) {
					swap(vector[j], vector[j + 1]);
					isSwapped = true;
				}
			}
			if (!isSwapped)
			{
				break;
			}
		}
	}


	// Get discretized point
	static Point2D<int>discretizePoint(const Point2D<double> point, const Size2D<double> cellSize)
	{
		double dx = cellSize.width;
		double dy = cellSize.height;

		int x = round(point.x / dx);
		int y = round(point.y / dy);

		return Point2D<int>(x, y);
	}


	// Get discretized size
	static Size2D<int> discretizeSize(const Size2D<double> size, const Size2D<double> cellSize)
	{
		double dx = cellSize.width;
		double dy = cellSize.height;

		int width = round(size.width / dx);
		int height = round(size.height / dy);

		return Size2D<int>(width, height);
	}


	//Round double number
	static double roundToDouble(double a, int round = roundOrder)
	{
		return std::round(a * roundOrder) / round;
	}


private:

	// Number of significant digits in rounding
	static const int roundOrder = 1e6;

};

#endif TOOL_H