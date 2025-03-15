#pragma once

namespace Types {

	// Absolute vacuum dielectric constant [F/m]
	const long double e0 = 8.854187818814e-12;

	// Absolute vacuum magnetic [N/A^2]
	const long double mu0 = 1.2566370612720e-6;

	const long double pi = 3.1415926535;

	// The speed of light [m/s]
	const long double speedLight = 2.999792458e8;

	// Initial potential for signal conductor cells
	const double initHightPotential = 1.0;

	// Initial potential for non-signal conductor cells
	const double initLowPotential = 0.0;

	// Cell info type
	struct CellInfo
	{
		double dx = 1.0;
		double dy = 1.0;
		double dielectricValue = 1.0;
		double tgValue = 0.0;
		double mu = 0.0;
		double ro = 0.0;
		bool isConductor = false;
		bool isSignalConductor = false;
		bool isHorizontalDielectricBound = false;
		bool isVerticalDielectricBound = false;

		bool operator == (CellInfo cellInfo)
		{
			if (this->dielectricValue == cellInfo.dielectricValue &&
				this->isConductor == cellInfo.isConductor &&
				this->isSignalConductor == cellInfo.isSignalConductor)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	// Linear parameters
	struct LinearParameters
	{
		long double C = 0.0;
		long double CAir = 0.0;
		long double L = 0.0;
		long double G = 0.0;

		long double CAirExtension = 0.0;
		long double LExtension = 0.0;
		long double R = 0.0;
	};

	struct PrimaryParameters
	{
		long double C1 = 0.0;
		long double C1Air = 0.0;
		long double epsEff = 0.0;
		long double Z0 = 0.0;
		long double Z1 = 0.0;
	};
}
