#include "Main.h"

int main()
{
	try
	{
		std::string stripStructureFile;

		std::cout << "Enter the full file name with strip structure config (json format):\n";
		std::cout << "> ";
		std::cin >> stripStructureFile;

		StripStructure stripStructure = StructureCreator::createStripStructureFromJson(stripStructureFile);

		std::cout << "\n";
		std::cout << "JSON parse succeeds";

		if (Tool::printShapesInfo == true)
		{
			std::cout << "\n";
			std::cout << "All materials and shapes have been created: \n\n";
			for (int i = 0; i < stripStructure.getShapes().getLength(); i++)
			{
				Shape2D* shape = stripStructure.getShapes()[i];

				std::cout << *shape;

				if (i < stripStructure.getShapes().getLength() - 1)
				{
					std::cout << "\n\n";
				}
			}
		}

		std::cout << "\n\n";

		stripStructure.computeElectroStaticAnalysis();
	}
	catch (const char* errorMsg)
	{
		std::cout << "\n";
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}