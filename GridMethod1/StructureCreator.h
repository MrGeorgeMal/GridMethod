#pragma once
#ifndef STRUCTURECREATOR_H
#define STRUCTURECREATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include "libs/json.hpp"

#include "StripStructure.h"
#include "Material.h"
#include "Shape2D.h"
#include "Point2D.h"
#include "Size2D.h"
#include "Vector"

using json = nlohmann::json;

namespace StructureCreator
{
	// Create strip structure from json file
	StripStructure createStripStructureFromJson(std::string path)
	{
		StripStructure stripStructure;

		// fix file path
		std::string charsForRemove = R"(")";
		std::string fixPath = "";

		for (char c : path) {
			if (charsForRemove.find(c) == std::string::npos) {
				fixPath += c;
			}
		}

		std::ifstream file(fixPath);
		if (!file.is_open())
		{
			throw "StructureCreator::createStripStructureFromJson::Failed to open file";
			return stripStructure;
		}
		
		try
		{
			json config;
			file >> config; // parse json from file
			
			Vector<Material*> materials;

			for (auto& jmaterial: config["materials"])
			{
				std::string type = jmaterial["type"];
				std::string name = jmaterial["name"];

				if (type == "conductor")
				{
					bool isSignal = jmaterial["mode"];
					Conductor* newConductor = new Conductor(isSignal);
					newConductor->setName(name);

					materials.add(newConductor);
				}
				if (type == "dielectric")
				{
					double value = jmaterial["value"];
					Dielectric* newDielectric = new Dielectric(value);
					newDielectric->setName(name);

					materials.add(newDielectric);
				}
			}

			Vector<Shape2D*> shapes;
			for (auto& jshape : config["shapes"])
			{
				std::string type = jshape["type"];
				std::string materialName = jshape["material"];

				Material* material = nullptr;

				for (int i = 0; i < materials.getLength(); i++)
				{
					if (materials[i]->getName() == materialName)
					{
						material = materials[i];
						break;
					}
				}
				if (material == nullptr)
				{
					throw "StructureCreator::createStripStructureFromJson::Bad material name";
				}

				if (type == "rectangle")
				{
					Point2D<double> point;
					Size2D<double> size;
					point.x = jshape["point"][0];
					point.y = jshape["point"][1];
					size.width = jshape["size"][0];
					size.height = jshape["size"][1];

					Rectangle2D* newRectangle = new Rectangle2D(point, size);
					newRectangle->setMaterial(material);
					shapes.add(newRectangle);
				}
				if (type == "line")
				{
					Point2D<double> p1;
					Point2D<double> p2;
					p1.x = jshape["p1"][0];
					p1.y = jshape["p1"][1];
					p2.x = jshape["p2"][0];
					p2.y = jshape["p2"][1];

					Line2D* newLine = new Line2D(p1, p2);
					newLine->setMaterial(material);
					shapes.add(newLine);
				}
			}

			for (int i = 0; i < shapes.getLength(); i++)
			{
				stripStructure.addShape(shapes[i]);
			}
		}
		catch (const json::parse_error& e)
		{
			std::cout << "JSON parse error\n" << e.what() << std::endl;
			throw "StructureCreator::createStripStructureFromJson::Parse failed";
		}
		
		return stripStructure;
	}
}

#endif STRUCTURECREATOR_H