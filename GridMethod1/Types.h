#pragma once

namespace Types {

	// Absolute dielectric constant of vacuum [F/m]
	const double e0 = 8.854187818814e-12;

	// Cell info type
	struct CellInfo
	{
		double potential = 0.0;
		double dielectricValue = 1.0;
		bool isConductor = false;
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
