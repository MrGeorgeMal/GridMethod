#pragma once
#include "StripStructure.h"

static class GridMethod
{
public:
	static void SetStartPotential(StripStructure* stripStructure)
	{
		for (int y = 0; y < stripStructure->GetFieldMatrixRows(); y++)
		{
			for (int x = 0; x < stripStructure->GetFieldMatrixCols(); x++)
			{
				stripStructure->GetFieldMatrix()[y][x].potentialValue = 0.0;
			}
		}

		for (int i = 0; i < stripStructure->GetSignalConductorsPoints().size(); i++)
		{
			for (int j = 0; j < stripStructure->GetSignalConductorsPoints()[i].size(); j++)
			{
				int x = stripStructure->GetSignalConductorsPoints()[i][j].x;
				int y = stripStructure->GetSignalConductorsPoints()[i][j].y;
				stripStructure->GetFieldMatrix()[y][x].potentialValue = 1.0;
			}
		}
	}

	static void CaclulateFieldMatrix(StripStructure* stripStructure)
	{
		double eps = 1e-6;

		SetStartPotential(stripStructure);

		int iterationsNumber = 0;
		double maxDeltaPotential = 1.0;
		while (eps < maxDeltaPotential)
		{
			iterationsNumber++;
			maxDeltaPotential = 0.0;
			for (int i = 0; i < stripStructure->GetAreas().size(); i++)
			{
				int startX = stripStructure->GetAreas()[i].startDisX;
				int startY = stripStructure->GetAreas()[i].startDisY;
				int x1 = stripStructure->GetAreas()[i].disX1;
				int y1 = stripStructure->GetAreas()[i].disY1;
				int x2 = stripStructure->GetAreas()[i].disX2;
				int y2 = stripStructure->GetAreas()[i].disY2;

				// Calculate Center
				double tempPotential = stripStructure->GetFieldMatrix()[startY][startX].potentialValue;
				double newPotential = CalcPoint(stripStructure, startX, startY);
				stripStructure->GetFieldMatrix()[startY][startX].potentialValue = newPotential;
				if (abs(newPotential - tempPotential) > maxDeltaPotential)
				{
					maxDeltaPotential = abs(newPotential - tempPotential);
				}

				// Calculate Vertical Center
				for (int y = startY - 1; y >= y1; y--)
				{
					tempPotential = stripStructure->GetFieldMatrix()[y][startX].potentialValue;
					newPotential = CalcPoint(stripStructure, startX, y);
					stripStructure->GetFieldMatrix()[y][startX].potentialValue = newPotential;
					if (abs(newPotential - tempPotential) > maxDeltaPotential)
					{
						maxDeltaPotential = abs(newPotential - tempPotential);
					}
				}
				for (int y = startY + 1; y < y1 + (y2 - y1); y++)
				{
					tempPotential = stripStructure->GetFieldMatrix()[y][startX].potentialValue;
					newPotential = CalcPoint(stripStructure, startX, y);
					stripStructure->GetFieldMatrix()[y][startX].potentialValue = newPotential;
					if (abs(newPotential - tempPotential) > maxDeltaPotential)
					{
						maxDeltaPotential = abs(newPotential - tempPotential);
					}
				}

				// Calculate Horizontal Center
				for (int x = startX - 1; x >= x1; x--)
				{
					tempPotential = stripStructure->GetFieldMatrix()[startY][x].potentialValue;
					newPotential = CalcPoint(stripStructure, x, startY);
					stripStructure->GetFieldMatrix()[startY][x].potentialValue = newPotential;
					if (abs(newPotential - tempPotential) > maxDeltaPotential)
					{
						maxDeltaPotential = abs(newPotential - tempPotential);
					}
				}
				for (int x = startX + 1; x < x1 + (x2 - x1); x++)
				{
					tempPotential = stripStructure->GetFieldMatrix()[startY][x].potentialValue;
					newPotential = CalcPoint(stripStructure, x, startY);
					stripStructure->GetFieldMatrix()[startY][x].potentialValue = newPotential;
					if (abs(newPotential - tempPotential) > maxDeltaPotential)
					{
						maxDeltaPotential = abs(newPotential - tempPotential);
					}
				}

				// Calculate left bottom
				for (int y = startY - 1; y >= y1; y--)
				{
					for (int x = startX - 1; x >= x1; x--)
					{
						tempPotential = stripStructure->GetFieldMatrix()[y][x].potentialValue;
						newPotential = CalcPoint(stripStructure, x, y);
						stripStructure->GetFieldMatrix()[y][x].potentialValue = newPotential;
						if (abs(newPotential - tempPotential) > maxDeltaPotential)
						{
							maxDeltaPotential = abs(newPotential - tempPotential);
						}
					}
				}

				// Calculate left top
				for (int y = startY + 1; y < y1 + (y2 - y1); y++)
				{
					for (int x = startX - 1; x >= x1; x--)
					{
						tempPotential = stripStructure->GetFieldMatrix()[y][x].potentialValue;
						newPotential = CalcPoint(stripStructure, x, y);
						stripStructure->GetFieldMatrix()[y][x].potentialValue = newPotential;
						if (abs(newPotential - tempPotential) > maxDeltaPotential)
						{
							maxDeltaPotential = abs(newPotential - tempPotential);
						}
					}
				}

				// Calculate right top
				for (int y = startY + 1; y < y1 + (y2 - y1); y++)
				{
					for (int x = startX + 1; x < x1 + (x2 - x1); x++)
					{
						tempPotential = stripStructure->GetFieldMatrix()[y][x].potentialValue;
						newPotential = CalcPoint(stripStructure, x, y);
						stripStructure->GetFieldMatrix()[y][x].potentialValue = newPotential;
						if (abs(newPotential - tempPotential) > maxDeltaPotential)
						{
							maxDeltaPotential = abs(newPotential - tempPotential);
						}
					}
				}

				// Calculate right bottom
				for (int y = startY - 1; y >= y1; y--)
				{
					for (int x = startX + 1; x < x1 + (x2 - x1); x++)
					{
						tempPotential = stripStructure->GetFieldMatrix()[y][x].potentialValue;
						newPotential = CalcPoint(stripStructure, x, y);
						stripStructure->GetFieldMatrix()[y][x].potentialValue = newPotential;
						if (abs(newPotential - tempPotential) > maxDeltaPotential)
						{
							maxDeltaPotential = abs(newPotential - tempPotential);
						}
					}
				}
			}
			//std::cout << "Max Delta Potential: " << maxDeltaPotential << "\n";
		}
		std::cout << "Number of iterations: " << iterationsNumber << "\n";
	}

	static void CalculateEnergy(Material** field, int rows, int cols)
	{
		double energy = 0.0;
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{

			}
		}
	}

private:
	static double CalcPoint(StripStructure* stripStructure, int x, int y)
	{
		if (stripStructure->GetFieldMatrix()[y][x].materialType != Material::EMaterialType::ScreenConductor &&
			stripStructure->GetFieldMatrix()[y][x].materialType != Material::EMaterialType::SignalConductor)
		{
			double leftU = 0.0;
			double topU = 0.0;
			double rightU = 0.0;
			double bottomU = 0.0;

			double leftE = 1.0;
			double topE = 1.0;
			double rightE = 1.0;
			double bottomE = 1.0;

			if (x - 1 >= 0)
			{
				leftU = stripStructure->GetFieldMatrix()[y][x - 1].potentialValue;
				leftE = stripStructure->GetFieldMatrix()[y][x - 1].dielectricValue;
			}
			if (y + 1 < stripStructure->GetFieldMatrixRows())
			{
				topU = stripStructure->GetFieldMatrix()[y + 1][x].potentialValue;
				topE = stripStructure->GetFieldMatrix()[y + 1][x].dielectricValue;
			}
			if (x + 1 < stripStructure->GetFieldMatrixCols())
			{
				rightU = stripStructure->GetFieldMatrix()[y][x + 1].potentialValue;
				rightE = stripStructure->GetFieldMatrix()[y][x + 1].dielectricValue;
			}
			if (y - 1 >= 0)
			{
				bottomU = stripStructure->GetFieldMatrix()[y - 1][x].potentialValue;
				bottomE = stripStructure->GetFieldMatrix()[y - 1][x].dielectricValue;
			}

			double result = (2 * leftU * leftE) / (leftE + rightE);
			result += (2 * rightU * rightE) / (leftE + rightE);
			result += (2 * topU * topE) / (topE + bottomE);
			result += (2 * bottomU * bottomE) / (topE + bottomE);
			result *= 0.25;
			return result;
		}
		else
		{
			return stripStructure->GetFieldMatrix()[y][x].potentialValue;
		}
	}
};