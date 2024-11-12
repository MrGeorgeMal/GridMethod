#pragma once

struct Material
{
	enum EMaterialType
	{
		None,
		Dielectric,
		Conductor
	};

	EMaterialType materialType = EMaterialType::None;
	double dielectricValue = 1.0;
	double potential = 0.0;
};