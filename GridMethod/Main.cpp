#include "Main.h"

void MakeDatasetAI(int dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		StripStructure* s = new StripStructure();
		GeneratorStructure::Generate(10.0, 10.0);
		for (int j = 0; j < GeneratorStructure::stripObjects.size(); j++)
		{
			s->AddObject(GeneratorStructure::stripObjects[j]);
		}
		s->BuildFieldMatrix(0.1, 0.1);
		s->PrintStripStructure();
		std::cout << "\n";
		s->SaveStructure("DataAI\\InputMatrix\\inputstruct.csv");

		GridMethod::SetStartPotential_Basic(s);
		GridMethod::CalculateFieldMatrix(s, false);
		s->SaveFieldMatrixToFile("DataAI\\OutputMatrix\\outputmatrix.csv");

		delete s;
	}
}

int main()
{
	Screen* screen = new Screen();
	Rectangle *rect = new Rectangle();
	Line *line1 = new Line();
	Line* line2 = new Line();
	StripStructure *stripStruct = new StripStructure();

	try
	{
		MakeDatasetAI(10000);

		/*
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
		

		GeneratorStructure::Generate(10.0, 10.0);
		for (int i = 0; i < GeneratorStructure::stripObjects.size(); i++)
		{
			stripStruct->AddObject(GeneratorStructure::stripObjects[i]);
		}

		stripStruct->BuildFieldMatrix(0.1, 0.1);

		stripStruct->PrintStripStructure();

		//GridMethod::CalculatePartialMatrix(stripStruct);

		stripStruct->PrintStructureInfo();

		delete stripStruct;
		*/
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}