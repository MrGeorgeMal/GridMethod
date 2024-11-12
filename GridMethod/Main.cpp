#include "Main.h"

int main()
{
	Screen* screen = new Screen();
	Rectangle *rect = new Rectangle();
	Line *line = new Line();
	StripStructure stripStruct = StripStructure();

	try
	{
		Material dielectric, conductor;
		dielectric.materialType = Material::EMaterialType::Dielectric;
		conductor.materialType = Material::EMaterialType::Conductor;

		screen->SetObject(dielectric, 0.0, 0.0, 20.0, 20.0);
		rect->SetObject(dielectric, 0.0, 5.0, 20.0, 10.0);
		line->SetObject(conductor, 7.0, 15.0, 6.0, 15.0);

		stripStruct.AddObject(*screen);
		stripStruct.AddObject(*rect);
		stripStruct.AddObject(*line);

		stripStruct.UpdateFieldMatrix();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}