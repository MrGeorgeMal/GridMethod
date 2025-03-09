#pragma once

namespace Types {

	// Absolute dielectric constant of vacuum [F/m]
	const long double e0 = 8.854187818814e-12;

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
		double dielectricValue = 1.0;
		double tgValue = 0.0;
		bool isConductor = false;
		bool isSignalConductor = false;


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
		long double R = 0.0;
		long double G = 0.0;
	};

}
