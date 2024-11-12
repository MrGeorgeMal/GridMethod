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
		conductor.potentialValue = 1.0;

		screen->SetObject(air, 20.0, 10.0);
		rect->SetObject(dielectric, 0.0, 4.0, 20.0, 1.5);
		line->SetObject(conductor, 8.0, 4.0 + 1.5, 12.0, 4.0 + 1.5);

		stripStruct->AddObject(screen);
		stripStruct->AddObject(rect);
		stripStruct->AddObject(line);

		stripStruct->UpdateFieldMatrix(0.5, 0.5);
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