#include "Main.h"

int main()
{
	Screen* screen = new Screen();
	Rectangle *rect = new Rectangle();
	Line *line1 = new Line();
	Line* line2 = new Line();
	Line* line3 = new Line();
	Line* line4 = new Line();
	StripStructure *stripStruct = new StripStructure();

	try
	{
		Material air, dielectric, conductor;
		air.materialType = Material::EMaterialType::Dielectric;
		dielectric.materialType = Material::EMaterialType::Dielectric;
		dielectric.dielectricValue = 4.5;
		conductor.materialType = Material::EMaterialType::SignalConductor;

		screen->SetObject(air, 20.0, 10.0);
		rect->SetObject(dielectric, 0.0, 4.0, 20.0, 1.5);
		line1->SetObject(conductor, 4.0, 4.0 + 1.5, 8.0, 4.0 + 1.5);
		line2->SetObject(conductor, 8.0, 4.0 + 1.5, 16.0, 4.0 + 1.5 + 3);
		line3->SetObject(conductor, 4.0, 4, 8.0, 4);
		line4->SetObject(conductor, 12.0, 4, 18.0, 4);

		line3->align = Line::ELineAlign::RightBottom;
		line4->align = Line::ELineAlign::RightBottom;

		stripStruct->AddObject(screen);
		stripStruct->AddObject(rect);
		stripStruct->AddObject(line1);
		stripStruct->AddObject(line2);
		stripStruct->AddObject(line3);
		stripStruct->AddObject(line4);

		stripStruct->BuildFieldMatrix(0.1, 0.1);

		stripStruct->PrintStructureInfo();
		stripStruct->PrintStripStructure();

		GridMethod::CaclulateFieldMatrix(stripStruct);
		stripStruct->SaveFieldMatrixToFile();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}