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
		
		int x = floor(point.x / dx);
		int y = floor(point.y / dy);

		return Point2D<int>(x, y);
	}


	// Get discretized size
	static Size2D<int> discretizeSize(const Size2D<double> size, const Size2D<double> cellSize)
	{
		double dx = cellSize.width;
		double dy = cellSize.height;

		int width = floor(size.width / dx);
		int height = floor(size.height / dy);

		return Size2D<int>(width, height);
	}


	//Round double number
	static double roundToDouble(double a, int round = roundOrder)
	{
		return std::round(a * pow(10, round)) / pow(10, round);
	}


	// Define digits count after dot
	static int defineDigitsCountAfterDot(double number)
	{
		std::string numstr = std::to_string(number);

		int digitsAfterDot = 0;
		int zeroCount = 0;
		int nineCount = 0;
		int maxError = roundOrder;
		for (int j = numstr.find('.') + 1; j < numstr.length(); j++)
		{
			digitsAfterDot++;
			if (numstr[j] == '0')
			{
				zeroCount++;
			}
			else
			{
				zeroCount = 0;
			}
			if (numstr[j] == '9')
			{
				nineCount++;
			}
			else
			{
				nineCount = 0;
			}

			if (zeroCount >= maxError || nineCount >= maxError)
			{
				digitsAfterDot -= zeroCount;
				break;
			}
		}

		return digitsAfterDot;
	}


private:

	// Number of significant digits in rounding
	static const int roundOrder = 3;

};

#endif TOOL_H