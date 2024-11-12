#include "Main.h"

int main()
{
	Rectangle rect = Rectangle();
	Line line = Line();

	try
	{
		Material dielectric, conductor;
		dielectric.materialType = Material::EMaterialType::Dielectric;
		conductor.materialType = Material::EMaterialType::Conductor;

		rect.SetObject(dielectric, 0.0, 0.0, 10.0, 5.0);
		rect.GetFieldMatrixFragment(1.0, 1.0);
		rect.PrintFieldMatrixFragment();

		std::cout << "\n\n";

		line.SetObject(conductor, 0.0, 0.0, 5.0, 10.0);
		line.GetFieldMatrixFragment(1.0, 1.0);
		line.PrintFieldMatrixFragment();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}