#include "Main.h"

int main()
{
	Screen* screen = new Screen();
	Rectangle *rect = new Rectangle();
	Line *line = new Line();
	StripStructure *stripStruct = new StripStructure();

	try
	{
		Material air, dielectric, conductor;
		air.materialType = Material::EMaterialType::Dielectric;
		dielectric.materialType = Material::EMaterialType::Dielectric;
		dielectric.dielectricValue = 4.5;
		conductor.materialType = Material::EMaterialType::Conductor;

		screen->SetObject(air, 40.0, 20.0);
		rect->SetObject(dielectric, 4.0, 5.0, 10.0, 10.0);
		line->SetObject(conductor, 7.0, 15.0, 13.0, 15.0);

		

		stripStruct->AddObject(screen);
		stripStruct->AddObject(rect);
		stripStruct->AddObject(line);

		stripStruct->UpdateFieldMatrix();
		stripStruct->PrintStripStructure();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}