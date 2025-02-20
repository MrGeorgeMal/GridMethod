#include "Main.h"

int main()
{
	try
	{		
		Dielectric* diel1 = new Dielectric(4.3);
		Dielectric* diel2 = new Dielectric(3.2);
		Conductor* cond1 = new Conductor(false);

		std::cout << &cond1 << "\n";

		Line2D line1(Point2D<double>(0.0, 0.0), Point2D<double>(2.0, 2.0));
		Polygon2D poly1;
		Rectangle2D rect1;

		line1.setMaterial(cond1);
		poly1.setMaterial(diel1);
		rect1.setMaterial(diel2);

		std::cout << line1.getMaterial()->getType() << "\n";
		std::cout << poly1.getMaterial()->getType() << "\n";
		std::cout << rect1.getMaterial()->getType() << "\n";

		Rasterizer rast(Size2D<double>(1.0, 1.0));
		rast.rasterize(&line1);
		rast.rasterize(&poly1);
		rast.rasterize(&rect1);
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}