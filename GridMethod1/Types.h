#pragma once

namespace Types {

	// Absolute dielectric constant of vacuum [F/m]
	const double e0 = 8.854187818814e-12;

	// Initial potential for signal conductor cells
	const double initHightPotential = 1.0;

	// Initial potential for non-signal conductor cells
	const double initLowPotential = 0.0;

	// Cell info type
	struct CellInfo
	{
		double dielectricValue = 1.0;
		bool isConductor = false;
		bool isSignalConductor = false;
	};

	// Linear parameters
	struct LinearParameters
	{
		double C = 0.0;
		double CAir = 0.0;
		double L = 0.0;
		double R = 0.0;
		double G = 0.0;
	};

}
