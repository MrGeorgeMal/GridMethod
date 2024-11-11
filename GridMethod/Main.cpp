#include "Main.h"

int main()
{
	Rectangle rect = Rectangle();

	try
	{
		rect.SetObject(Material(), 0.0, 0.0, 10.0, 10.0);
		rect.GetFieldMatrixFragment(1, 1);
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}