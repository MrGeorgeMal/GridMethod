#include "Main.h"

int main()
{
	Screen* screen = new Screen();
	Rectangle *rect = new Rectangle();
	Line *line1 = new Line();
	Line* line2 = new Line();
	StripStructure *stripStruct = new StripStructure();

	try
	{
		Material air, dielectric, signalConductor, screenConductor;
		air.materialType = Material::EMaterialType::Dielectric;
		dielectric.materialType = Material::EMaterialType::Dielectric;
		dielectric.dielectricValue = 4.5;
		signalConductor.materialType = Material::EMaterialType::SignalConductor;
		screenConductor.materialType = Material::EMaterialType::ScreenConductor;

		screen->SetObject(air, 19.0, 10.0);
		rect->SetObject(dielectric, 0.0, 4.0, 19.0, 1.5);
		line1->SetObject(signalConductor, 8.0, 4.0 + 1.5, 11.0, 4.0 + 1.5);
		line2->SetObject(screenConductor, 0.0, 4.0, 19.0, 4.0);

		line2->align = Line::ELineAlign::RightBottom;

		stripStruct->AddObject(screen);
		stripStruct->AddObject(rect);
		stripStruct->AddObject(line1);
		stripStruct->AddObject(line2);

		stripStruct->BuildFieldMatrix(0.1, 0.1);

		stripStruct->PrintStripStructure();

		GridMethod::CalculatePartialMatrix(stripStruct);

		stripStruct->PrintStructureInfo();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}