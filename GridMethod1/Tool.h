#pragma once
#ifndef TOOL_H
#define TOOL_H

#include <string>

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

	// Get string from double number
	static std::string doubleToString(double number)
	{
		std::string str = "";

		if (outputDigitsCount <= 0)
		{
			return str;
		}

		bool isNegetive = false;
		if (number < 0.0)
		{
			str += "-";
			number *= -1;
		}

		double temp = number;
		int rate = 0;

		if (temp != 0.0)
		{
			if ((int)floor(temp) == 0)
			{
				while ((int)floor(temp) == 0)
				{
					temp *= 10;
					rate--;
				}
			}
			else
			{
				while ((int)floor(temp) != 0)
				{
					temp /= 10;
					rate++;
				}
				temp *= 10;
				rate--;
			}
		}

		temp = round(temp * pow(10, outputDigitsCount)) / pow(10, outputDigitsCount);

		if (rate >= -minDigitsForExponent && rate < 0)
		{
			temp = round(number * pow(10, outputDigitsCount)) / pow(10, outputDigitsCount);
		}

		int fixDigitsCount = outputDigitsCount;

		if (rate > 0 && rate <= minDigitsForExponent)
		{
			fixDigitsCount += rate;
		}

		for (int i = 0; i <= fixDigitsCount; i++)
		{
			double tempNumber = temp * pow(10, i);

			std::string tempStr = std::to_string(int(tempNumber));
			str += tempStr.back();

			if (rate > 0 && rate <= minDigitsForExponent)
			{
				if (i == abs(rate))
				{
					str += ".";
				}
			}
			else
			{
				if (i == 0)
				{
					str += ".";
				}
			}
		}

		if (rate != 0 && (rate > minDigitsForExponent || rate < -minDigitsForExponent))
		{
			str += "e";
			str += std::to_string(rate);
		}

		return str;
	}


private:

	// Number of significant digits in rounding
	static const int roundOrder = 3;

public:
	// Digits count for output numbers
	inline static int outputDigitsCount = 6;

	// Min digits for exponent formation double number
	inline static int minDigitsForExponent = 3;

	// Draw field for GridSolver or not
	inline static bool drawField = false;

	// Draw rasterized strip structure or not
	inline static bool drawStructure = false;

	// Print shapes info
	inline static bool printShapesInfo = false;

	// Compute conductors loss matrix or not
	inline static bool lossless = false;
};

#endif TOOL_H