#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <string>

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
	Material() : _materialType(EType::NONE), _name("material") {}

	// Constructor
	// materialType - type of material (Material::EType)
	Material(EType materialType) : _materialType(materialType) {}

	EType getMaterialType() const { return _materialType; }

	// Get object type
	virtual const char* getType() const = 0;

	// Set material name
	void setName(std::string name) { _name = name; }

	// Get material name
	std::string getName() const { return _name; }

	// Print material info
	virtual std::ostream& print(std::ostream& os) const = 0;

protected:

	// Type of material
	EType _materialType;

	std::string _name;
};



// Class dielectric material
class Dielectric : public Material
{
public:
	// Base constructor. Set permittivity 1.0, loss tangent 0.0
	Dielectric() : Material(EType::DIELECTRIC), _dielectricValue(1.0), _tgValue(0.0) {}

	// Constructor
	// _dielectricValue - dielectric permittivity value
	Dielectric(double dielectricValue) : Material(EType::DIELECTRIC), _dielectricValue(dielectricValue), _tgValue(0.0) {}

	// Constructor
	// _dielectricValue - dielectric permittivity value
	// _tgValue - loss tangent
	Dielectric(double dielectricValue, double tgValue) : Material(EType::DIELECTRIC), _dielectricValue(dielectricValue), _tgValue(tgValue) {}

	double getDielectricValue() { return _dielectricValue; }

	double getTgValue() { return _tgValue; }

	const char* getType() const override { return "Dielectric"; }
	
	std::ostream& print(std::ostream& os) const override
	{
		os << "[";
		os << "Dielectric ; ";
		os << _name << " ; ";
		os << _dielectricValue << " ; ";
		os << _tgValue << "]";
		return os;
	}

private:
	// Permittivity value
	double _dielectricValue;
	double _tgValue;
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

	std::ostream& print(std::ostream& os) const override
	{
		os << "[";
		os << "Conductor ; ";
		os << _name << " ; ";
		const char* str = (_isSignal == true) ? "signal mode" : "ground mode";
		os << str;
		os << "]";
		return os;
	}

private:
	bool _isSignal;
};



inline std::ostream& operator<<(std::ostream& os, const Material& material) {
	return material.print(os);
}

#endif // !MATERIAL_H
