#pragma once
#include "StripStructure.h"
#include "chrono"

static class GridMethod
{
public:
	static void CalculatePartialMatrix(StripStructure* stripStructure)
	{
		int conductorsNumber = stripStructure->GetSignalConductorsPoints().size();
		switch (conductorsNumber)
		{
		case 0:
			throw "GridMethod::BadSignalConductorsNumber(Non-Conductors)";
			break;
		case 1:
			CalculatePartialMatrix_OneConductor(stripStructure);
			break;
		case 2:
			CalculatePartialMatrix_TwoConductor(stripStructure);
			break;
		default:
			CalculatePartialMatrix_ThreeOrMoreConductor(stripStructure);
			break;
		}
	}

	static void CalculatePartialMatrix_OneConductor(StripStructure* stripStructure)
	{
		int iterations;
		std::chrono::steady_clock::time_point start, end;
		std::chrono::milliseconds msec;

		start = std::chrono::high_resolution_clock::now();
		SetStartPotential_OneConductor(stripStructure);
		iterations = CalculateFieldMatrix(stripStructure, false);
		CalculateEnergy(stripStructure, false);
		end = std::chrono::high_resolution_clock::now();
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "GridMethod calculation info:\n";
		std::cout << "One conductor\n";
		std::cout << "Dielectric filling\n";
		std::cout << "Iteration of grid method: " << iterations << "\n";
		std::cout << "Time: " << msec.count() << " milliseconds\n\n";
		stripStructure->SaveFieldMatrixToFile("Outputs\\OneConductor_Dielectric.csv");

		start = std::chrono::high_resolution_clock::now();
		SetStartPotential_OneConductor(stripStructure);
		iterations = CalculateFieldMatrix(stripStructure, true);
		CalculateEnergy(stripStructure, true);
		end = std::chrono::high_resolution_clock::now();
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "GridMethod calculation info:\n";
		std::cout << "One conductor\n";
		std::cout << "Air filling\n";
		std::cout << "Iteration of grid method: " << iterations << "\n";
		std::cout << "Time: " << msec.count() << " milliseconds\n\n";
		stripStructure->SaveFieldMatrixToFile("Outputs\\OneConductor_Air.csv");
	}

	static void CalculatePartialMatrix_TwoConductor(StripStructure* stripStructure)
	{
		SetStartPotential_TwoConductor(stripStructure, 1.0, 1.0);
		CalculateFieldMatrix(stripStructure, false);
		CalculateEnergy(stripStructure, false);

		SetStartPotential_TwoConductor(stripStructure, 1.0, 1.0);
		CalculateFieldMatrix(stripStructure, true);
		CalculateEnergy(stripStructure, true);

		SetStartPotential_TwoConductor(stripStructure, 1.0, -1.0);
		CalculateFieldMatrix(stripStructure, false);
		CalculateEnergy(stripStructure, false);

		SetStartPotential_TwoConductor(stripStructure, 1.0, -1.0);
		CalculateFieldMatrix(stripStructure, true);
		CalculateEnergy(stripStructure, true);
	}

	static void CalculatePartialMatrix_ThreeOrMoreConductor(StripStructure* stripStructure)
	{
		throw "GridMethod::The calculation of strip structures with three or more signal conductors is under development.";
		return;
	}

	static void SetStartPotential_OneConductor(StripStructure* stripStructure)
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

	static void SetStartPotential_TwoConductor(StripStructure* stripStructure, double FirstPotential, double SecondPotential)
	{
		for (int y = 0; y < stripStructure->GetFieldMatrixRows(); y++)
		{
			for (int x = 0; x < stripStructure->GetFieldMatrixCols(); x++)
			{
				stripStructure->GetFieldMatrix()[y][x].potentialValue = 0.0;
			}
		}

		int conductorNumber = 0;
		for (int j = 0; j < stripStructure->GetSignalConductorsPoints()[conductorNumber].size(); j++)
		{
			int x = stripStructure->GetSignalConductorsPoints()[conductorNumber][j].x;
			int y = stripStructure->GetSignalConductorsPoints()[conductorNumber][j].y;
			stripStructure->GetFieldMatrix()[y][x].potentialValue = FirstPotential;
		}
		conductorNumber = 1;
		for (int j = 0; j < stripStructure->GetSignalConductorsPoints()[conductorNumber].size(); j++)
		{
			int x = stripStructure->GetSignalConductorsPoints()[conductorNumber][j].x;
			int y = stripStructure->GetSignalConductorsPoints()[conductorNumber][j].y;
			stripStructure->GetFieldMatrix()[y][x].potentialValue = SecondPotential;
		}
	}

	static int CalculateFieldMatrix(StripStructure* stripStructure, bool isAir)
	{
		double eps = 1e-6;

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
				double newPotential = CalcPoint(stripStructure, startX, startY, isAir);
				stripStructure->GetFieldMatrix()[startY][startX].potentialValue = newPotential;
				if (abs(newPotential - tempPotential) > maxDeltaPotential)
				{
					maxDeltaPotential = abs(newPotential - tempPotential);
				}

				// Calculate Vertical Center
				for (int y = startY - 1; y >= y1; y--)
				{
					tempPotential = stripStructure->GetFieldMatrix()[y][startX].potentialValue;
					newPotential = CalcPoint(stripStructure, startX, y, isAir);
					stripStructure->GetFieldMatrix()[y][startX].potentialValue = newPotential;
					if (abs(newPotential - tempPotential) > maxDeltaPotential)
					{
						maxDeltaPotential = abs(newPotential - tempPotential);
					}
				}
				for (int y = startY + 1; y < y1 + (y2 - y1); y++)
				{
					tempPotential = stripStructure->GetFieldMatrix()[y][startX].potentialValue;
					newPotential = CalcPoint(stripStructure, startX, y, isAir);
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
					newPotential = CalcPoint(stripStructure, x, startY, isAir);
					stripStructure->GetFieldMatrix()[startY][x].potentialValue = newPotential;
					if (abs(newPotential - tempPotential) > maxDeltaPotential)
					{
						maxDeltaPotential = abs(newPotential - tempPotential);
					}
				}
				for (int x = startX + 1; x < x1 + (x2 - x1); x++)
				{
					tempPotential = stripStructure->GetFieldMatrix()[startY][x].potentialValue;
					newPotential = CalcPoint(stripStructure, x, startY, isAir);
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
						newPotential = CalcPoint(stripStructure, x, y, isAir);
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
						newPotential = CalcPoint(stripStructure, x, y, isAir);
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
						newPotential = CalcPoint(stripStructure, x, y, isAir);
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
						newPotential = CalcPoint(stripStructure, x, y, isAir);
						stripStructure->GetFieldMatrix()[y][x].potentialValue = newPotential;
						if (abs(newPotential - tempPotential) > maxDeltaPotential)
						{
							maxDeltaPotential = abs(newPotential - tempPotential);
						}
					}
				}
			}
		}
		return iterationsNumber;
	}

	static void CalculateEnergy(StripStructure* stripStructure, bool isAir)
	{
		double energy = 0.0;
		for (int y = 0; y < stripStructure->GetFieldMatrixRows(); y++)
		{
			for (int x = 0; x < stripStructure->GetFieldMatrixCols(); x++)
			{
				double leftBottomU = stripStructure->GetFieldMatrix()[y][x].potentialValue;
				double rightTopU = 0.0;
				double rightBottomU = 0.0;
				double leftTopU = 0.0;

				double leftBottomE = 1.0;
				if (!isAir) leftBottomE = stripStructure->GetFieldMatrix()[y][x].dielectricValue;

				if (y + 1 <= stripStructure->GetFieldMatrixRows() - 1 && x + 1 <= stripStructure->GetFieldMatrixCols() - 1)
				{
					rightTopU = stripStructure->GetFieldMatrix()[y + 1][x + 1].potentialValue;
				}
				if (x + 1 <= stripStructure->GetFieldMatrixCols() - 1)
				{
					rightBottomU = stripStructure->GetFieldMatrix()[y][x + 1].potentialValue;
				}
				if (y + 1 <= stripStructure->GetFieldMatrixRows() - 1)
				{
					leftTopU = stripStructure->GetFieldMatrix()[y + 1][x].potentialValue;
				}
				double firstCross = (leftBottomU - rightTopU) * (leftBottomU - rightTopU);
				double secondCross = (rightBottomU - leftTopU) * (rightBottomU - leftTopU);
				energy += (leftBottomE / 4) * (firstCross + secondCross);
			}
		}
		stripStructure->AddEnergy(energy);
	}

private:
	static double CalcPoint(StripStructure* stripStructure, int x, int y, bool isAir)
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
				if (!isAir) leftE = stripStructure->GetFieldMatrix()[y][x - 1].dielectricValue;
			}
			if (y + 1 < stripStructure->GetFieldMatrixRows())
			{
				topU = stripStructure->GetFieldMatrix()[y + 1][x].potentialValue;
				if (!isAir) topE = stripStructure->GetFieldMatrix()[y + 1][x].dielectricValue;
			}
			if (x + 1 < stripStructure->GetFieldMatrixCols())
			{
				rightU = stripStructure->GetFieldMatrix()[y][x + 1].potentialValue;
				if (!isAir) rightE = stripStructure->GetFieldMatrix()[y][x + 1].dielectricValue;
			}
			if (y - 1 >= 0)
			{
				bottomU = stripStructure->GetFieldMatrix()[y - 1][x].potentialValue;
				if (!isAir) bottomE = stripStructure->GetFieldMatrix()[y - 1][x].dielectricValue;
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