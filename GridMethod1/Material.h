#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>

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

	// Get object type
	virtual const char* getType() const = 0;

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

	const char* getType() const override { return "Dielectric"; }

	friend std::ostream& operator<<(std::ostream& os, Dielectric& material)
	{
		os << "[";
		os << "Dielectric";
		os << " ; " << material.getDielectricValue() << "]";
		return os;
	}

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

	const char* getType() const override { return "Conductor"; }

	friend std::ostream& operator<<(std::ostream& os, Conductor& material)
	{
		os << "[";
		os << "Conductor";
		const char* str = (material.isSignal() == true) ? "signal mode" : "free mode";
		os << " ; " << str;
		os << "]";
		return os;
	}

private:
	bool _isSignal;
};


/*
// Convert material type to string
const char* materialTypeToString(Material* material)
{
	switch (material->getMaterialType())
	{
	case Material::EType::NONE:
		return "None";
		break;
	case Material::EType::DIELECTRIC:
		return "Dielectric";
		break;
	case Material::EType::CONDUCTOR:
		return "Conductor";
		break;
	default:
		return "Wrong material";
		break;
	}
}
*/

/*
std::ostream& operator<<(std::ostream& os, Dielectric& material)
{
	os << "[";
	//os << materialTypeToString(material);
	os << " ; " << material.getDielectricValue() << "]";
	return os;
}
*/

#endif // !MATERIAL_H
