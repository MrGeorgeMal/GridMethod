#pragma once

// Class material
class Material
{
public:
	// Enum of material types
	enum EType
	{
		NONE,
		DIELECTRIC,
		CONDUCTOR
	};

	// Base constructor
	Material() : _materialType(EType::NONE) {}

	// Constructor
	// materialType - type of material (Material::EType)
	Material(EType materialType) : _materialType(materialType) {}

	EType getMaterialType() { return _materialType; }

protected:
	// Type of material
	EType _materialType;
};



// Class dielectric material
class Dielectric : public Material
{
public:
	// Base constructor. Set permittivity 1.0
	Dielectric() : Material(EType::DIELECTRIC), _dielectricValue(1.0) {}

	// Constructor
	// _dielectricValue - dielectric permittivity value
	Dielectric(double dielectricValue) : Material(EType::DIELECTRIC), _dielectricValue(dielectricValue) {}

	double getDielectricValue() { return _dielectricValue; }

private:
	// Permittivity value
	double _dielectricValue;
};



// Class conductor material
class Conductor : public Material
{
public:
	// Base constructor. Set signal off
	Conductor() : Material(EType::CONDUCTOR), _isSignal(false) {}

	// Constructor
	// isSignal - set operating mode of conductor
	Conductor(bool isSignal) : Material(EType::CONDUCTOR), _isSignal(isSignal) {}

	bool isSignal() { return _isSignal; }

private:
	bool _isSignal;
};



std::ostream& operator<<(std::ostream& os, Material& material)
{
	switch (material.getMaterialType())
	{
	case Material::EType::NONE:
		os << "None";
		break;
	case Material::EType::DIELECTRIC:
		os << "Dielectric";
		break;
	case Material::EType::CONDUCTOR:
		os << "Conductor";
		break;
	default:
		os << "Wrong material";
		break;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, Dielectric& material)
{
	os << "[";
	switch (material.getMaterialType())
	{
	case Material::EType::NONE:
		os << "None";
		break;
	case Material::EType::DIELECTRIC:
		os << "Dielectric";
		break;
	case Material::EType::CONDUCTOR:
		os << "Conductor";
		break;
	default:
		os << "Wrong material";
		break;
	}
	os << " ; " << material.getDielectricValue() << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, Conductor& material)
{
	os << "[";
	switch (material.getMaterialType())
	{
	case Material::EType::NONE:
		os << "None";
		break;
	case Material::EType::DIELECTRIC:
		os << "Dielectric";
		break;
	case Material::EType::CONDUCTOR:
		os << "Conductor";
		break;
	default:
		os << "Wrong material";
		break;
	}
	const char* str = (material.isSignal() == true) ? "signal mode" : "free mode";
	os << " ; " << str;
	os << "]";
	return os;
}