#include "Main.h"

int main()
{
	try
	{
		Vector<int> v(3);
		for (int i = 0; i < v.getLength(); i++)
		{
			v[i] = i;
		}

		std::cout << v << "\n";


		Dielectric diel1(4.3);
		Dielectric diel2(3.1);
		Conductor cond1(false);

		Line2D line1(Point2D<double>(0.0, 0.0), Point2D<double>(0.0, 0.0));
		Polygon2D poly1;
		Rectangle2D rect1;

		line1.setMaterial(&cond1);
		poly1.setMaterial(&diel1);
		rect1.setMaterial(&diel2);

		std::cout << cond1 << "\n";
		std::cout << line1.getMaterial()->getType() << "\n";
		std::cout << poly1.getMaterial()->getType() << "\n";
		std::cout << rect1.getMaterial()->getType() << "\n";

		Rasterizer rast(Size2D<double>(1.0, 1.0));
		rast.Rasterize(line1);
		rast.Rasterize(poly1);
		rast.Rasterize(rect1);
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}