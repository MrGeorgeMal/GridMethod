#pragma once

struct Material
{
	enum EMaterialType
	{
		None,
		Dielectric,
		ScreenConductor,
		SignalConductor,
	};

	EMaterialType materialType = EMaterialType::None;
	double dielectricValue = 1.0;
	double potentialValue = 0.0;
};